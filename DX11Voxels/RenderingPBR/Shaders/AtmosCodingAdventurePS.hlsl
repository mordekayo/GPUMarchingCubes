
#define M_PI 3.1415926535897932384626433832795


cbuffer ConstBuf : register(b0)
{
    float4x4 invVP;

    float3 viewPos;   // Position of the viewer
	float PlanetRadius;

	float3 PlanetCenter;
	float AtmosphereRadius;

	float3 sunDir;    // Direction to the sun
	float I_sun;    // Intensity of the sun
    
	float DensityFalloff;
	int numScatteringPoints;
	int numOpticalDepthPoints;
	float dummy0;
}


Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D MetRougAoMap : register(t2);
Texture2D EmissiveMap : register(t3);
Texture2D WorldPosMap : register(t4);

Texture2D AccumMap : register(t5);
//Texture2D BloomMap : register(t6);


struct GBufferData
{
    float4 Diffuse;
    float3 Normal;
    float4 MetRougAo;
    float3 Emissive;
    float4 WorldPos;
};


float2 raySphere(float3 sphereCenter, float sphereRadius, float3 rayOrigin, float3 rayDir)
{
    float3 offset = rayOrigin - sphereCenter;
    float a = 1;
    float b = 2 * dot(offset, rayDir);
    float c = dot(offset, offset) - sphereRadius * sphereRadius;
    float d = b * b - 4 * a * c;

    if (d > 0) {
        float s = sqrt(d);
        float dstToSphereNear = max(0, (-b - s) / (2 * a));
        float dstToSphereFar = (-b + s) / (2 * a);

        if (dstToSphereFar >= 0) {
            return float2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
        }
    }

    float maxFloat = 100000000.0f;
    return float2(maxFloat, 0);
}


float densityAtPoint(float3 densitySamplePoint)
{
	float heightAboveSurface = length(densitySamplePoint - PlanetCenter) - PlanetRadius;
	float height01 = heightAboveSurface / (AtmosphereRadius - PlanetRadius);
	float localDensity = exp(-height01 * DensityFalloff) * (1.0f - height01);

	return localDensity;

}

float opticalDepth(float3 rayOrigin, float3 rayDir, float rayLength)
{
	float3 densitySamplePoint = rayOrigin;
	float stepSize = rayLength / (numOpticalDepthPoints - 1);
	float opticalDepth = 0;
    
	for (int i = 0; i < numOpticalDepthPoints; ++i)
	{
		float localDensity = densityAtPoint(densitySamplePoint);
        
		opticalDepth += localDensity * stepSize;
		densitySamplePoint += rayDir * stepSize;
	}
    
	return opticalDepth;

}


float calculateLight(float3 rayOrigin, float3 rayDir, float rayLength)
{
    float3 inScatterPoint = rayOrigin;

	float stepSize = rayLength / (numScatteringPoints - 1);
    float inScatteredLight = 0;

	for (int i = 0; i < numScatteringPoints; ++i)
    {
        float sunRayLength = raySphere(PlanetCenter, AtmosphereRadius, inScatterPoint, sunDir).y;
        float sunRayOpticalDepth = opticalDepth(inScatterPoint, sunDir, sunRayLength);
		float viewRayOpticalDepth = opticalDepth(inScatterPoint, -rayDir, stepSize*i);
        
		float transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth));
		float localDensity = densityAtPoint(inScatterPoint);
        
		inScatteredLight += localDensity * transmittance * stepSize;
        inScatterPoint += rayDir * stepSize;
    }
	return inScatteredLight;
}



GBufferData ReadGBuffer(float2 screenPos)
{
    GBufferData buf = (GBufferData)0;

    buf.Diffuse = DiffuseMap.Load(float3(screenPos, 0));
    buf.WorldPos = WorldPosMap.Load(float3(screenPos, 0));
    buf.Emissive = EmissiveMap.Load(float3(screenPos, 0)).xyz;
    buf.Normal = NormalMap.Load(float3(screenPos, 0)).xyz;
    buf.MetRougAo = MetRougAoMap.Load(float3(screenPos, 0));

    return buf;
}




[earlydepthstencil]
float4 PSMain(in float4 pos : SV_POSITION, in float4 pPos : TEXCOORD0) : SV_Target0
{
    GBufferData data = ReadGBuffer(pos.xy);
    float4 acc = AccumMap.Load(float3(pos.xy, 0));

    float4 wPos = mul(pPos, invVP);
    wPos /= wPos.w;

    float3 viewDir = normalize(wPos.xyz - viewPos);
    float sceneDepth = data.WorldPos.w;

    float2 hitInfo = raySphere(PlanetCenter, AtmosphereRadius, viewPos, viewDir);
    float dstToAtmosphere = hitInfo.x;
    float dstThroughAtmosphere = min(hitInfo.y, sceneDepth - dstToAtmosphere);

    
	if (dstThroughAtmosphere > 0)
	{
		float3 pointInAtmosphere = viewPos + viewDir * dstToAtmosphere;
		float light = calculateLight(pointInAtmosphere, viewDir, dstThroughAtmosphere);
		return float4(acc.rgb * (1.0f - light) + light, 1.0f);
	}
    
	return float4(acc.rgb, 1.0f);
    
	//float t = dstThroughAtmosphere / (AtmosphereRadius * 2);
    //float3 color = t.xxx ;
    //
    //return float4(color, 1.0f);
}
