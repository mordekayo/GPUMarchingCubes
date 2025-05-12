// ---------------------------------------------------------------------------
// Global structs
// ---------------------------------------------------------------------------

struct VS_OUTPUT
{
	float4 vClipPos									: SV_Position;
	float2 vTexCoord								: TEXCOORD0;
	float3 vEyeToPixel								: TEXCOORD1;
};

// ---------------------------------------------------------------------------
// Global variables
// ---------------------------------------------------------------------------

cbuffer PS_CBUFFER									: register (b0)
{
	float3 g_sunDirection;
	float g_sunIntensity;
	float g_skyRotation;
	float g_rayleighScattering;
	float g_mieScattering;
	float g_useDynamicSkyTexture;
}

Texture2D g_textureDynamicSkyDome					: register(t0);
SamplerState g_samplerBilinear						: register(s0);

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------

float3 ToneMapping(float3 color)
{
	float A = 0.22;
	float B = 0.30;
	float C = 0.1;
	float D = 0.2;
	float E = 0.01;
	float F = 0.3;
	return ((color*(A*color + C*B) + D*E) / (color*(A*color + B) + D*F)) - E / F;
}

#define PI 3.141592

// ---------------------------------------------------------------------------
// Pixel shader
// ---------------------------------------------------------------------------
float4 main(VS_OUTPUT In) : SV_Target
{
	//return float4(1.0f, 0.0f, 0.0f, 1.0);

	float3 V = normalize(In.vEyeToPixel);
	float3 L = normalize(g_sunDirection);
	float sun_disk = pow(max(0.0, (dot(V, L) - 0.99995)/0.00005), 1.001); // cos(0.57 deg) = 0.99995: Our sun disk is ~2 times larger than it is in real life
	//sun_disk = 0.0f;

	// Getting sky color
	float2 uv = (V.xz / (1.0 + V.y))*(0.5 / 1.5) + 0.5;
	float3 sky_color = g_textureDynamicSkyDome.SampleLevel(g_samplerBilinear, uv, 0).rgb;
	sky_color *= g_sunIntensity;
	
	// Getting sun color
	float3 sun_color = g_textureDynamicSkyDome.SampleLevel(g_samplerBilinear, uv, 0).rgb * g_sunIntensity * sun_disk * 20.0;

	// Combining sky color and sun disk
	float3 color = sky_color + sun_color;
	//float3 color = sun_color;
	
	//color = float3(0, 0, 0);
	//float ExposureBias = 1.0;
	//float3 curr = ToneMapping(ExposureBias*color);
	
	//float3 whiteScale = 1.0 / ToneMapping(3.0);
	//color = curr * whiteScale;

	//color = pow(color, 1 / 2.2);
	return float4(color, 1.0);
} 
