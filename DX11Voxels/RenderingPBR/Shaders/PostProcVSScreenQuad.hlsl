
struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 pPos : TEXCOORD0;
};


PS_IN VSMain(uint id: SV_VertexID)
{
	PS_IN output = (PS_IN)0;
	float2 inds = float2(id & 1, (id & 2) >> 1);
	output.pos	= float4(inds * float2(2, -2) + float2(-1, 1), 0, 1);
	output.pPos = output.pos;

	return output;
}
