
struct VSOutput
{
	int vertexID : TEXCOORD0;
};

struct GSOutput
{
	float4 	Position 	: SV_Position;
	float3 	WPos		: TEXCOORD0;
	uint 	RTIndex 	: SV_RenderTargetArrayIndex;
};


VSOutput VSMain(uint vertexID : SV_VertexID)
{
	VSOutput output;
	output.vertexID = vertexID;
	return output;
}


[maxvertexcount(24)]
void GSMain(point VSOutput inputPoint[1], inout TriangleStream<GSOutput> outputStream)
{
	GSOutput p0, p1, p2, p3;

	p0.Position = float4(-1.0f,	1.0f, 1.0f, 1.0f);	// TopLeft
	p1.Position = float4(1.0f,	1.0f, 1.0f, 1.0f);	// TopRight
	p2.Position = float4(-1.0f,-1.0f, 1.0f, 1.0f);	// BottomLeft
	p3.Position = float4(1.0f, -1.0f, 1.0f, 1.0f);	// BottomRight
	
	
	////////////////// RIGHT
	p0.RTIndex = 0;
	p1.RTIndex = 0;
	p2.RTIndex = 0;
	p3.RTIndex = 0;
	
	p0.WPos = float3(1.0f, 1.0f, 1.0f);
	p1.WPos = float3(1.0f, 1.0f, -1.0f);
	p2.WPos = float3(1.0f, -1.0f, 1.0f);
	p3.WPos = float3(1.0f, -1.0f, -1.0f);

	outputStream.Append(p0); outputStream.Append(p1); outputStream.Append(p2); outputStream.Append(p3); outputStream.RestartStrip();
	
	/////////////////// LEFT
	p0.RTIndex = 1;
	p1.RTIndex = 1;
	
	p0.WPos = float3(-1.0f, 1.0f, -1.0f);
	p1.WPos = float3(-1.0f, 1.0f, 1.0f);
	p2.WPos = float3(-1.0f, -1.0f, -1.0f);
	p3.WPos = float3(-1.0f, -1.0f, 1.0f);
	
	outputStream.Append(p0); outputStream.Append(p1); outputStream.Append(p2); outputStream.Append(p3); outputStream.RestartStrip();
	
	/////////////////// TOP
	p0.RTIndex = 2;
	p1.RTIndex = 2;
	
	p0.WPos = float3(-1.0f, 1.0f, -1.0f);
	p1.WPos = float3(1.0f, 1.0f, -1.0f);
	p2.WPos = float3(-1.0f, 1.0f, 1.0f);
	p3.WPos = float3(1.0f, 1.0f, 1.0f);
	
	outputStream.Append(p0); outputStream.Append(p1); outputStream.Append(p2); outputStream.Append(p3); outputStream.RestartStrip();
	
	/////////////////// BOTTOM
	p0.RTIndex = 3;
	p1.RTIndex = 3;
	
	p0.WPos = float3(-1.0f, -1.0f, 1.0f);
	p1.WPos = float3(1.0f, -1.0f, 1.0f);
	p2.WPos = float3(-1.0f, -1.0f, -1.0f);
	p3.WPos = float3(1.0f, -1.0f, -1.0f);
	
	outputStream.Append(p0); outputStream.Append(p1); outputStream.Append(p2); outputStream.Append(p3); outputStream.RestartStrip();
	
	//////////////////// FRONT
	p0.RTIndex = 5;
	p1.RTIndex = 5;
	
	p0.WPos = float3(1.0f,	1.0f, -1.0f);
	p1.WPos = float3(-1.0f,	1.0f, -1.0f);
	p2.WPos = float3(1.0f, -1.0f, -1.0f);
	p3.WPos = float3(-1.0f, -1.0f, -1.0f);
	
	outputStream.Append(p0); outputStream.Append(p1); outputStream.Append(p2); outputStream.Append(p3); outputStream.RestartStrip();
	
	//////////////////// BACK
	p0.RTIndex = 4;
	p1.RTIndex = 4;
	
	p0.WPos = float3(-1.0f,	1.0f, 1.0f);
	p1.WPos = float3(1.0f,	1.0f, 1.0f);
	p2.WPos = float3(-1.0f, -1.0f, 1.0f);
	p3.WPos = float3(1.0f, -1.0f, 1.0f);
	
	outputStream.Append(p0); outputStream.Append(p1); outputStream.Append(p2); outputStream.Append(p3); outputStream.RestartStrip();
}



////////////////////////// SphericalToCube /////////////////////////////////////
Texture2D SphereMap : register(t0);
SamplerState Sampler : register(s0);

#define PI 3.14159265359f


float2 SampleSphericalMap(float3 v)
{
	const float2 invAtan = float2(0.1591, 0.3183);

    float2 uv = float2(atan2(v.x, v.z), asin(v.y));
    uv *= invAtan;
	uv.x = 0.5f - uv.x;
	uv.y = 0.5f - uv.y;
    return uv;
}


float4 PSSphericalToCube(GSOutput input) : SV_Target0
{
	float2 uv = SampleSphericalMap(normalize(input.WPos));
	float4 color = SphereMap.SampleLevel(Sampler, uv, 0);

	//return float4(normalize(input.WPos.xyz), 1.0f);
	return color;
}
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
TextureCube EnvMap : register(t0);
SamplerState EnvSampler : register(s0);

struct ConstantData
{
	float4 RoughnessXXX;
};

cbuffer ConstBuf : register(b0)
{
	ConstantData ConstData;
}


float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
float2 Hammersley(uint i, uint N)
{
	return float2(float(i) / float(N), RadicalInverse_VdC(i));
}
// ----------------------------------------------------------------------------
float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
	float a = roughness * roughness;
	float alpha2 = a * a;
	
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha2 - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
    // from spherical coordinates to cartesian coordinates
	float3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
	float3 up = abs(N.y) < 0.9 ? float3(0.0, 1.0, 0.0) : float3(0.0, 0.0, -1.0);
	float3 tangent = normalize(cross(up, N));
	float3 bitangent = cross(tangent, N);
	
	float3 sampleVec = tangent * H.x * 1.0f + bitangent * H.y * 1.0f + N * H.z;
	return normalize(sampleVec);
}


float DistributionGGX(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	NdotH = saturate(NdotH);
	float NdotH2 = NdotH * NdotH;
	
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return num / denom;
}


float4 PSPreFilterCube2(GSOutput input) : SV_Target0
{
	float3 N = normalize(input.WPos);
	float3 R = N;
	float3 V = R;

	const uint SAMPLE_COUNT = 1024u;
	float totalWeight = 0.0;
	float3 prefilteredColor = float3(0.0f, 0.0f, 0.0f);
	for (uint i = 0u; i < SAMPLE_COUNT; ++i)
	{
		float2 Xi = Hammersley(i, SAMPLE_COUNT);
		float3 H = ImportanceSampleGGX(Xi, N, ConstData.RoughnessXXX.x);
		float3 L = normalize(2.0 * dot(V, H) * H - V);

		float NdotL = max(dot(N, L), 0.0);
		if (NdotL > 0.0)
		{
			prefilteredColor += EnvMap.Sample(EnvSampler, L).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	prefilteredColor = prefilteredColor / totalWeight;

	return float4(prefilteredColor, 1.0);
}

 
 float4 PSPreFilterCube(GSOutput input) : SV_Target0
{
    float3 res = (float3)0.0f;  
    float totalWeight = 0.0f;   
     
    float3 normal = normalize(input.WPos);
    float3 toEye = normal;
     
    float roughness = ConstData.RoughnessXXX.x; // max(0.02f,roughness);
     
	static const uint NUM_SAMPLES = 1024u;
    for(uint i = 0; i < NUM_SAMPLES; ++i)
    {
		float2 xi = Hammersley(i, NUM_SAMPLES);
         
		float3 halfway = ImportanceSampleGGX(xi, normal, roughness);
        float3 lightVec = 2.0f * dot( toEye,halfway ) * halfway - toEye;
         
        float NdotL = saturate ( dot( normal, lightVec ) ) ;
        //float NdotV = saturate ( dot( normal, toEye ) ) ;
        float NdotH =  saturate ( dot( normal, halfway ) ) ;
        float HdotV = saturate ( dot( halfway, toEye ) ) ;
         
        if( NdotL > 0 )
        {
            float D = DistributionGGX(NdotH, roughness);
            float pdf = (D * NdotH / (4 * HdotV)) + 0.0001f  ;

            float resolution = 2048.0; // resolution of source cubemap (per face)
            float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
            float saSample = 1.0f / (NUM_SAMPLES * pdf + 0.00001f);
             
            float mipLevel = roughness == 0.0f ? 0.0f :  0.5f * log2( saSample / saTexel )  ;
                                 
            res += EnvMap.SampleLevel( EnvSampler, lightVec, mipLevel ).rgb * NdotL;     
            totalWeight += NdotL;
        }
    }
     
    return float4(res / max(totalWeight, 0.001f), 1.0);
}
//*/

