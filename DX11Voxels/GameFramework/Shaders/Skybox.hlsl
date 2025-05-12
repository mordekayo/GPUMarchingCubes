
struct ConstantData
{
	float4x4 World; 
	float4x4 View;
	float4x4 Proj;
};

cbuffer ConstBuf : register(b0) {
	ConstantData ConstData;
}

struct VS_IN
{
	float4 pos		: POSITION;
	float4 tex		: COLOR0;
};

struct PS_IN
{
	float4 pos		: SV_POSITION;
	float4 wPos		: TEXCOORD0;
};

TextureCube		DiffuseMap		: register(t0);
SamplerState	Sampler			: register(s0);

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

    output.pos = mul(float4(input.pos.xyz, 1.0f), ConstData.World);
	output.wPos = output.pos;
    output.pos = mul(output.pos, ConstData.View);
    output.pos = mul(output.pos, ConstData.Proj);

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 color = DiffuseMap.SampleLevel(Sampler, normalize(input.wPos.xyz), 1);
	//color.rgb = pow(color.rgb, 1.0f/2.2f);
	return float4(color.rgb, 0.0f);
}
