
struct ConstantData
{
	float4x4 WorldViewProj;
};

cbuffer ConstBuf : register(b0) {
	ConstantData ConstData;
}

struct VS_IN
{
	float2 pos : POSITION0;
	float2 tex : TEXCOORD0;
    float4 col : COLOR0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
    float4 col : COLOR;
};

Texture2D		DiffuseMap		: register(t0);
SamplerState	Sampler			: register(s0);


PS_IN VSMain( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	
	output.pos = mul(float4(input.pos.xy, 0.0f, 1.0f), ConstData.WorldViewProj);
	output.tex = input.tex.xy;
    output.col = input.col;

	return output;
}

float4 PSMain( PS_IN input ) : SV_Target
{
    float4 color = DiffuseMap.SampleLevel(Sampler, input.tex.xy, 0) * input.col;

	return color;
}
