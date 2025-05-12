
#define M_PI 3.1415926535897932384626433832795


cbuffer ConstBuf : register(b0)
{
    float4x4 invVP;

    float3 viewPos;   // Position of the viewer
    float g;		// Mie scattering direction - anisotropy of the medium

    float3 sunPos;    // Position of the sun, light direction
    float I_sun;    // Intensity of the sun

    // Number of samples along the view ray and light ray
    int viewSamples;
    int lightSamples;
    float R_e;      // Radius of the planet [m]
    float R_a;      // Radius of the atmosphere [m]

    float3  beta_R;   // Rayleigh scattering coefficient
    float beta_M;   // Mie scattering coefficient

    float H_R;      // Rayleigh scale height
    float H_M;      // Mie scale height

    float2 dummy;
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


float2 raySphereIntersection(float3 o, float3 d, float r)
{
    // Solving analytically as a quadratic function
    //  assumes that the sphere is centered at the origin
    // f(x) = a(x^2) + bx + c
    float a = dot(d, d);
    float b = 2.0 * dot(d, o);
    float c = dot(o, o) - r * r;

    // Discriminant or delta
    float delta = b * b - 4.0 * a * c;

    // Roots not found
    if (delta < 0.0) {
        // TODO
        return float2(1e5f, -1e5f);
    }

    float sqrtDelta = sqrt(delta);
    // TODO order??
    return float2((-b - sqrtDelta) / (2.0f * a), (-b + sqrtDelta) / (2.0f * a));
}


float3 computeSkyColor(float3 ray, float3 origin)
{
    // Normalize the light direction
    float3 sunDir = normalize(sunPos);

    float2 t = raySphereIntersection(origin, ray, R_a);
    // Intersects behind
    if (t.x > t.y) {
        return float3(0.0f, 0.0f, 0.0f);
    }

    // Distance between samples - length of each segment
    t.y = min(t.y, raySphereIntersection(origin, ray, R_e).x);
    float segmentLen = (t.y - t.x) / float(viewSamples);

    // TODO t min
    float tCurrent = 0.0f;

    // Rayleigh and Mie contribution
    float3 sum_R = float3(0, 0, 0);
    float3 sum_M = float3(0, 0, 0);

    // Optical depth 
    float optDepth_R = 0.0;
    float optDepth_M = 0.0;

    // Mu: the cosine angle between the sun and ray direction
    float mu = dot(ray, sunDir);
    float mu_2 = mu * mu;

    //--------------------------------
    // Rayleigh and Mie Phase functions
    float phase_R = 3.0 / (16.0 * M_PI) * (1.0 + mu_2);

    float g_2 = g * g;
    float phase_M = 3.0 / (8.0 * M_PI) *
        ((1.0 - g_2) * (1.0 + mu_2)) /
        ((2.0 + g_2) * pow(1.0 + g_2 - 2.0 * g * mu, 1.5));
    // Sample along the view ray
    for (int i = 0; i < viewSamples; ++i)
    {
        // Middle point of the sample position
        float3 vSample = origin + ray * (tCurrent + segmentLen * 0.5);

        // Height of the sample above the planet
        float height = length(vSample) - R_e;

        // Optical depth for Rayleigh and Mie scattering for current sample
        float h_R = exp(-height / H_R) * segmentLen;
        float h_M = exp(-height / H_M) * segmentLen;
        optDepth_R += h_R;
        optDepth_M += h_M;

        //--------------------------------
        // Secondary - light ray
        float segmentLenLight = raySphereIntersection(vSample, sunDir, R_a).y / float(lightSamples);
        float tCurrentLight = 0.0;

        // Light optical depth 
        float optDepthLight_R = 0.0;
        float optDepthLight_M = 0.0;

        // Sample along the light ray
        for (int j = 0; j < lightSamples; ++j)
        {
            // Position of the light ray sample
            float3 lSample = vSample + sunDir * (tCurrentLight + segmentLenLight * 0.5);
            // Height of the light ray sample
            float heightLight = length(lSample) - R_e;

            // TODO check sample above the ground

            optDepthLight_R += exp(-heightLight / H_R) * segmentLenLight;
            optDepthLight_M += exp(-heightLight / H_M) * segmentLenLight;

            // Next light sample
            tCurrentLight += segmentLenLight;
        }
        // TODO check sample above ground

        // Attenuation of the light for both Rayleigh and Mie optical depth
        //  Mie extenction coeff. = 1.1 of the Mie scattering coeff.
        float3 att = exp(-(beta_R * (optDepth_R + optDepthLight_R) + beta_M * 1.1f * (optDepth_M + optDepthLight_M)));
        // Accumulate the scattering 
        sum_R += h_R * att;
        sum_M += h_M * att;

        // Next view sample
        tCurrent += segmentLen;
    }

    return I_sun * (sum_R * beta_R * phase_R + sum_M * beta_M * phase_M);
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
    float4 final = float4(0.25f, 0.25f, 0.25f, 1.0f);
    float4 acc = AccumMap.Load(float3(pos.xy, 0));

    float4 wPos = mul(pPos, invVP);
    wPos /= wPos.w;

    float3 dir = normalize(wPos.xyz - viewPos);

    float3 color = computeSkyColor(dir, viewPos);

    float2 dist = raySphereIntersection(viewPos, dir, R_e);
    //float2 dist = raySphere(float3(0,0,0), 30, viewPos, dir);

    //if (dist.x > dist.y)
    //    color = float3(0.0f, 0.0f, 0.0f);
    //else
    //    color = float3(0.0f, 0.3f, 0.0f);

    return float4(color + acc.rgb * 0.05f, 1.0f);
}
