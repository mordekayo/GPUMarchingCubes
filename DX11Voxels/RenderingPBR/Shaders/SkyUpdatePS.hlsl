// ---------------------------------------------------------------------------
// Global structs
// ---------------------------------------------------------------------------

struct VS_OUTPUT
{
	float4 vPos                             : SV_Position;
	float2 vTexCoord                        : TEXCOORD0;
};



// ---------------------------------------------------------------------------
// Vertex shader
// ---------------------------------------------------------------------------

VS_OUTPUT VSMain(uint id : SV_VertexID)
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.vTexCoord = float2(id & 1, (id & 2) >> 1);
	output.vPos = float4(output.vTexCoord * float2(2, -2) + float2(-1, 1), 0, 1);

	//output.vTexCoord.y = 1.0f - output.vTexCoord.y;

	return output;
}



// ---------------------------------------------------------------------------
// Global variables
// ---------------------------------------------------------------------------

cbuffer PS_CBUFFER							: register (b0)
{
	float3 g_sunDirection;
	float g_sunIntensity;
	float g_skyRotation;
	float g_rayleighScattering;
	float g_mieScattering;
	float g_useDynamicSkyTexture;
}

Texture2D g_textureStaticSkyDome					: register(t0);
SamplerState g_samplerBilinear						: register(s0);

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Mie + Raileigh atmospheric scattering code 
// based on Sean O'Neil Accurate Atmospheric Scattering 
// from GPU Gems 2 
//-----------------------------------------------------------------------------

float2 ray_sphere_intersection(float3 position, float3 direction, float radius) 
{
	// assuming the center of the sphere is located at the origin
	float a = dot(direction, direction);
	float b = 2.0 * dot(direction, position);
	float c = dot(position, position) - (radius * radius);
	float d = (b*b) - 4.0*a*c;
	if (d < 0.0) return float2(1e5, -1e5);
	return float2((-b - sqrt(d)) / (2.0*a),	(-b + sqrt(d)) / (2.0*a));
}

#define PrimaryRaySteps 64
#define SecondaryRaySteps 16
#define PI 3.141592
#define Kr float3(5.8e-6, 13.5e-6, 33.1e-6)
#define Km 1e-5
#define G 0.88
#define PlanetRadius 6360000.0
#define AtmosphereRadius 6420000.0
#define AverageRayleighScatterHeight 8000.0
#define AverageMieScatterHeight 1200.0

float3 CalculateAtmosphericScattering(float3 V, float3 L) 
{
	float3 EyePos = float3(0, 0, PlanetRadius + 20.0);

	float2 Intersections = ray_sphere_intersection(EyePos, V, AtmosphereRadius);
	if (Intersections.x > Intersections.y) return float3(0, 0, 0);

	Intersections.y = min(Intersections.y, ray_sphere_intersection(EyePos, V, PlanetRadius).x);

	float3 RayleighColor = float3(0, 0, 0);
	float3 MieColor = float3(0, 0, 0);
	float3 TotalAttenuation = 0.0;

	float PrimaryRayRayleighOpticalDepth = 0.0;
	float PrimaryRayMieOpticalDepth = 0.0;

	float fcos = dot(V, L);
	float RayleighPhase = 3.0 / (16.0*PI) * (1.0 + fcos*fcos);
	float MiePhase = 3.0 / (8.0*PI) * ((1.0 - G*G) * (fcos*fcos + 1.0)) / (pow(1.0 + G*G - 2.0*fcos*G, 1.5) * (2.0 + G*G));

	float PrimaryRayStepsize = (Intersections.y - Intersections.x) / float(PrimaryRaySteps);
	float tPrimaryRay = 0.0;
	for (int i = 0; i < PrimaryRaySteps; i++) 
	{
		float3 PrimaryRayPosition = EyePos + V * (tPrimaryRay + PrimaryRayStepsize*0.5);

		float PrimaryRayHeight = length(PrimaryRayPosition) - PlanetRadius;

		float RayleighOpticalDepthStep = PrimaryRayStepsize * exp(-PrimaryRayHeight / AverageRayleighScatterHeight);
		float MieOpticalDepthStep = PrimaryRayStepsize * exp(-PrimaryRayHeight / AverageMieScatterHeight);

		PrimaryRayRayleighOpticalDepth += RayleighOpticalDepthStep;
		PrimaryRayMieOpticalDepth += MieOpticalDepthStep;

		float SecondaryRayStepsize = ray_sphere_intersection(PrimaryRayPosition, L, AtmosphereRadius).y / float(SecondaryRaySteps);

		float SecondaryRayRayleighOpticalDepth = 0.0;
		float SecondaryRayMieOpticalDepth = 0.0;

		float tSecondaryRay = 0.0;
		for (int j = 0; j < SecondaryRaySteps; j++) 
		{
			float3 SecondaryRayPosition = PrimaryRayPosition + L * (tSecondaryRay + SecondaryRayStepsize * 0.5);
			float SecondaryRayHeight = length(SecondaryRayPosition) - PlanetRadius;

			SecondaryRayRayleighOpticalDepth += SecondaryRayStepsize * exp(-SecondaryRayHeight / AverageRayleighScatterHeight);
			SecondaryRayMieOpticalDepth += SecondaryRayStepsize * exp(-SecondaryRayHeight / AverageMieScatterHeight);

			tSecondaryRay += SecondaryRayStepsize;
		}
		float3 Attenuation = exp(-(Km * (PrimaryRayMieOpticalDepth + SecondaryRayMieOpticalDepth) + Kr * (PrimaryRayRayleighOpticalDepth + SecondaryRayRayleighOpticalDepth)));

		RayleighColor += RayleighOpticalDepthStep * Attenuation;
		MieColor += MieOpticalDepthStep * Attenuation;
		TotalAttenuation += Attenuation;

		tPrimaryRay += PrimaryRayStepsize;
	}

	return RayleighPhase * Kr * RayleighColor + MiePhase * Km * MieColor;
}

float SolarRadianceApproximation(float3 L)
{
	float airmass = 1.5 / (L.z + 0.5);
	return pow(0.7, pow(airmass, 0.678));
}

// ---------------------------------------------------------------------------
// Pixel shader
// ---------------------------------------------------------------------------
float4 main(VS_OUTPUT In) : SV_Target
{
	if (g_useDynamicSkyTexture > 0)
	{
		// Transform from quad UV to dome space
		float2 uv = (2.0 * In.vTexCoord.xy - 1.0) * 1.5;

		float l = dot(uv, uv);

		float3 r = float3(2.0 * uv, 1.0 - l) / (1.0 + l);
		float a = r.z;
		if (l > 1.0)
		{
			l = min(1.0, 1.0*(l - 1.0));
			r = lerp(r, float3(0.0, 0.0, -1.0), l*l);
		}
		r = normalize(r);

		float3 L = normalize(g_sunDirection);
		float3 color = 25.0 * CalculateAtmosphericScattering(r, L) * SolarRadianceApproximation(L);
		//float3 color = SolarRadianceApproximation(L);
		/*
		float c = cos(uv.x* 50.0)*sin(uv.y*50.0);
		c = (c > 0.999) ? 10.0 : 0.0;
		color.rgb = c;
		*/

		return float4(color, 1.0);
	}
	else
	{
		// Transform from quad UV to dome space
		float2 uv = (2.0 * In.vTexCoord.xy - 1.0) * 1.5;

		float l = dot(uv, uv);

		float3 r = float3(2.0 * uv, 1.0 - l) / (1.0 + l);
		r = normalize(r);

		uv.x = atan2(r.y, r.x) / (2.0*PI);
		uv.x -= 0.2;
		if (uv.x < 0) uv.x += 1.0;
		uv.y = 1.0 - 2.0*asin(r.z) / PI;

		float3 color = 2.5 * g_textureStaticSkyDome.Sample(g_samplerBilinear, uv).bgr - 0.8;

		color = pow(color, 2.2);
		return float4(color, 1.0);
	}
} 
