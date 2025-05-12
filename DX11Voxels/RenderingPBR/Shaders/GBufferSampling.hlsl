
struct ConstParams
{
	float4 SamplePos;
};


cbuffer CB1 : register(b0)
{
	ConstParams Params;
};

Texture2D DiffuseMap	: register(t0);
Texture2D NormalMap		: register(t1);
Texture2D MetRougAoIdMap: register(t2);
Texture2D EmissiveMap	: register(t3);
Texture2D WorldPosMap	: register(t4);


struct GSampleData
{
	float3	WorldPosition;
	float	Distance;
	float3	WorldNormal;
	int		Id;
};

RWStructuredBuffer<GSampleData> sampleBuf : register(u0);


[numthreads(1, 1, 1)]
void SampleGBuffer()
{
	GSampleData data = (GSampleData) 0;
	float4 worldPos = WorldPosMap.Load(int3(Params.SamplePos.xy, 0));
	data.WorldPosition = worldPos.xyz;
	data.Distance = worldPos.w;
	data.WorldNormal = NormalMap.Load(int3(Params.SamplePos.xy, 0)).xyz;
	data.Id = MetRougAoIdMap.Load(int3(Params.SamplePos.xy, 0)).w;

	sampleBuf[0] = data;
}

