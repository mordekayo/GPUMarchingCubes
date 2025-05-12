
struct GSOutput
{
	float4 Position : SV_Position;
	float3 WPos : TEXCOORD0;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

TextureCube DiffuseMap : register(t0);
SamplerState Sampler : register(s0);


float4 PSMain(GSOutput input) : SV_Target0
{
	const float PI = 3.14159265359;
	
	//return float4(1,1,1,1.0f);
	
	float3 N = normalize(input.WPos.xyz);
	
	float3 irradiance = float3(0, 0, 0);
    
    // tangent space calculation from origin point
	float3 up = float3(0.0, 1.0, 0.0);
	float3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));
       
	float sampleDelta = 0.025f;
	float nrSamples = 0.0f;
	
	for (float phi = 0.0; phi < 2.0f * PI; phi += sampleDelta)
	{
		for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
		{
            // spherical to cartesian (in tangent space)
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
	
			irradiance += DiffuseMap.Sample(Sampler, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples += 1.0f;
		}
	}
	irradiance = PI * irradiance * (1.0f / float(nrSamples));
	
	return float4(irradiance, 1.0f);
}

