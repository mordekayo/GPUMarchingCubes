
struct ConstantData
{
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
	float4x4 WorldViewProj;
	float4x4 InvertTransposeWorld;
	float4 ViewerPos;
	float4 PerspectiveMatVals;
};


struct LightData
{
	float4 Pos;
	float4 Dir;
	float4 Params;
	float4 Color;
};


struct CascadeData
{
	float4x4	ViewProj[4];
	float4		Distances;
};


struct FogData
{
	float3	FogColor;
	float	FogStartDist;
	float3	FogHighlightColor;
	float	FogGlobalDensity;
	float3	FogSunDir;
	float	FogHeightFalloff;
};


cbuffer ConstBuf : register(b0)
{
	ConstantData ConstData;
}

cbuffer LightsBuf : register(b1)
{
	LightData Light;
}

cbuffer CascBuf : register(b2)
{
	CascadeData CascData;
}

cbuffer FogBuf : register(b3)
{
	FogData Fog;
}


struct VS_IN
{
	float4 pos : POSITION0;
	float4 color : COLOR0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
};


SamplerState Sampler : register(s0);
SamplerState SamplerClamp : register(s1);
SamplerComparisonState ShadowCompSampler : register(s2);


PS_IN VSMain(
#ifdef SCREEN_QUAD
	uint id: SV_VertexID
#else
	VS_IN input
#endif
)
{
#ifdef SCREEN_QUAD
	PS_IN output = (PS_IN)0;
	float2 inds = float2(id & 1, (id & 2) >> 1);
	output.pos = float4(inds * float2(2, -2) + float2(-1, 1), 0, 1);
#else
	PS_IN output = (PS_IN) 0;
	output.pos = mul(float4(input.pos.xyz, 1.0f), ConstData.WorldViewProj);
#endif
	
	return output;
}



struct PSOutput
{
	float4 Accumulator : SV_Target0;
	float4 Bloom : SV_Target1;
};


Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D MetRougAoMap : register(t2);
Texture2D EmissiveMap : register(t3);
Texture2D WorldPosMap : register(t4);

TextureCube PreFiltEnvMap : register(t5);
TextureCube ConMap : register(t6);
Texture2D IntegratedMap : register(t7);

Texture2DArray ShadowMap : register(t8);


struct GBufferData
{
	float4 Diffuse;
	float3 Normal;
	float4 MetRougAo;
	float3 Emissive;
	float4 WorldPos;
};


GBufferData ReadGBuffer(float2 screenPos)
{
	GBufferData buf = (GBufferData) 0;

	buf.Diffuse		= DiffuseMap.Load(float3(screenPos, 0));
	buf.WorldPos	= WorldPosMap.Load(float3(screenPos, 0));
	buf.Emissive	= EmissiveMap.Load(float3(screenPos, 0)).xyz;
	buf.Normal		= NormalMap.Load(float3(screenPos, 0)).xyz;
	buf.MetRougAo	= MetRougAoMap.Load(float3(screenPos, 0));

	return buf;
}


float ConvertDepthToLinear(float depth)
{
	float linearDepth = ConstData.PerspectiveMatVals.z / (depth + ConstData.PerspectiveMatVals.w);
	return linearDepth;
}


#define PI 3.1415926535

float GGX_PartialGeometry(float cosThetaN, float alpha)
{
	float cosTheta_sqr = saturate(cosThetaN * cosThetaN);
	float tan2 = (1.0f - cosTheta_sqr) / cosTheta_sqr;
	float GP = 2.0f / (1.0f + sqrt(1 + alpha * alpha * tan2));
	return GP;
}


float GGX_Distribution(float cosThetaNH, float alpha)
{
	float alpha2 = alpha * alpha;
	float NH_sqr = saturate(cosThetaNH * cosThetaNH);
	float den = NH_sqr * alpha2 + (1.0 - NH_sqr);
	return alpha2 / (PI * den * den);
}


float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return num / denom;
}


float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return num / denom;
}


float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}


float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}


float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	float invRoug = 1.0 - roughness;
	return F0 + (max(invRoug.xxx, F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


float offset_lookup(float2 loc, float layer, float cmpDepth, float2 offset)
{
	float depth = ShadowMap.SampleCmpLevelZero(ShadowCompSampler, float3(loc + offset, layer), cmpDepth).x;

	return depth;
}


float3 ApplyFog(float3 originalColor, float eyePosY, float3 eyeToPixel)
{
	float pixelDist = length(eyeToPixel);
	float3 eyeToPixelNorm = eyeToPixel / pixelDist;
	
// Find the fog staring distance to pixel distance
	float fogDist = max(pixelDist - Fog.FogStartDist, 0.0);
	
// Distance based fog intensity
	float fogHeightDensityAtViewer = exp(-Fog.FogHeightFalloff * eyePosY);
	float fogDistInt = fogDist * fogHeightDensityAtViewer;
	
// Height based fog intensity
	float eyeToPixelY = eyeToPixel.y * (fogDist / pixelDist);
	float t = Fog.FogHeightFalloff * eyeToPixelY;
	const float thresholdT = 0.01;
	float fogHeightInt = abs(t) > thresholdT ? (1.0 - exp(-t)) / t : 1.0;
	
// Combine both factors to get the final factor
	float fogFinalFactor = exp(-Fog.FogGlobalDensity * fogDistInt * fogHeightInt);
	
// Find the sun highlight and use it to blend the fog color
	float sunHighlightFactor = saturate(dot(eyeToPixelNorm, Fog.FogSunDir));
	sunHighlightFactor = pow(sunHighlightFactor, 8.0);
	float3 fogFinalColor = lerp(Fog.FogColor, Fog.FogHighlightColor, sunHighlightFactor);
	return lerp(fogFinalColor, originalColor, fogFinalFactor);
}


[earlydepthstencil]
PSOutput PSMain(PS_IN input)
{
	PSOutput ret = (PSOutput) 0;

	GBufferData buf = ReadGBuffer(input.pos.xy);

	clip(length(buf.Normal) - 0.001f);

#ifdef DirectionalLight
	float3 L = normalize(Light.Pos.xyz);
#else
	float3 L = normalize(Light.Pos.xyz - buf.WorldPos.xyz);
#endif
	
	float3 N = normalize(buf.Normal);
	float3 V = normalize(ConstData.ViewerPos.xyz - buf.WorldPos.xyz);
	float3 R = reflect(-V, N);
	float3 H = normalize(V + L);
	float3 albedo = buf.Diffuse.rgb;
	float metallic = buf.MetRougAo.r;
	float roughness = buf.MetRougAo.g;
	float ao = 1.0f;
	
	float NdotL = max(dot(N, L), 0.0);
	float NdotV = max(dot(N, V), 0.0);
	
	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, albedo, metallic);

#ifdef AmbientLight
	/////// Ambient part - Diffuse Irradiance ////////////////////////////////////////////////////////
	float3 Fa = fresnelSchlickRoughness(NdotV, F0, roughness);
	float3 kSa = Fa;
	float3 kDa = float3(1.0f, 1.0f, 1.0f) - kSa;
	kDa *= 1.0 - metallic;
	float3 irradiance = ConMap.Sample(Sampler, N).rgb;
	float3 diffusea = irradiance * albedo;


	// sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
	const float MAX_REFLECTION_LOD = 4.0;
	float3 prefilteredColor = PreFiltEnvMap.SampleLevel(Sampler, R, roughness * MAX_REFLECTION_LOD).rgb;
	float2 brdf = IntegratedMap.Sample(SamplerClamp, float2(NdotV, roughness)).rg;
	float3 speculara = prefilteredColor * (Fa * brdf.x + brdf.yyy);


	float3 ambient = (kDa * diffusea * Light.Params.x + speculara * Light.Params.y) * ao;

	ret.Accumulator = float4(ambient, 1.0f);
	ret.Bloom = float4(1.0f, 1.0f, 1.0f, 1.0f);
	/////////////////////////////////////////////////////////////////////////////
#else
	
#ifdef PointLight
	// calculate per-light radiance
	float distance = length(Light.Pos.xyz - buf.WorldPos.xyz);
	float attenuation = max(1.0f - distance / Light.Params.x, 0);
	attenuation *= attenuation;
	float3 radiance = attenuation * Light.Params.z;
#else
	float3 radiance = Light.Params.xxx;
#endif

	// cook-torrance brdf
	float	NDF	= DistributionGGX(N, H, roughness);
	float	G	= GeometrySmith(N, V, L, roughness);
	float3	F	= fresnelSchlick(max(dot(H, V), 0.0), F0);

	float3 kS = F;
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	kD *= 1.0 - metallic;

	float3 numerator = NDF * G * F;
	float denominator = 4.0 * NdotV * NdotL;
	float3 specular = numerator / max(denominator, 0.001);

	// reflectance equation
	float3 Lo = float3(0, 0, 0);
	Lo += (kD * albedo / PI + specular) * radiance * NdotL;
  
	float3 color = Lo;

#ifdef DirectionalLight

	float4 colors[4] = { float4(1.0f, 0.0f, 0.0f, 1.0f), float4(0.0f, 1.0f, 0.0f, 1.0f), float4(0.0f, 0.0f, 1.0f, 1.0f), float4(1.0f, 1.0f, 0.0f, 1.0f) };
	
	int layer = 3;
	float depthVal = abs(buf.WorldPos.w);
	for (int i = 0; i < 4; ++i) {
		if (depthVal < CascData.Distances[i]) {
			layer = i;
			break;
		}
	}

	float4 screenSpace = mul(float4(buf.WorldPos.xyz, 1.0f), CascData.ViewProj[layer]);
	screenSpace = screenSpace / screenSpace.w;

	float2 texCoords = (screenSpace.xy + float2(1.0f, 1.0f)) * 0.5f;
	texCoords.y = 1.0f - texCoords.y;

	float shWidth, shHeight, shElements;
	ShadowMap.GetDimensions(shWidth, shHeight, shElements);

	float xOff = 1.0f / shWidth;
	float yOff = 1.0f / shHeight;

	float sum = 0;
	for (float y = -1.5; y <= 1.5; y += 1.0)
		for (float x = -1.5; x <= 1.5; x += 1.0)
			sum += offset_lookup(texCoords, layer, screenSpace.z, float2(x * xOff, y * yOff));

	float shadowCoeff = sum / 16.0;
	
	color *= shadowCoeff;
	//color = colors[layer].rgb;
	
	color = ApplyFog(color, ConstData.ViewerPos.y, depthVal);
#endif

	ret.Accumulator = float4(color, 1.0f);

	float4 bloom = float4(1.0f, 1.0f, 1.0f, 1.0f);
	ret.Bloom = bloom;

#endif

	return ret;
}
