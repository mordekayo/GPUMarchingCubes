
struct CascadeData
{
	float4x4	ViewProj[4];
	float4		Distances;
};

struct ConstantData
{
	float4x4 World;
};

cbuffer ConstBuf : register(b0) // In VS
{
	ConstantData ConstData;
}

cbuffer CascBuf : register(b0) // In PS
{
	CascadeData CascData;
}

struct VS_IN
{
	float4 pos : POSITION0;
	float4 normal : NORMAL0;
	float4 binormal : BINORMAL0;
	float4 tangent : TANGENT0;
	float4 color : COLOR0;
	float4 tex : TEXCOORD0;
};

struct VS_IN_ANIM
{
	float4 pos : POSITION0;
	float4 normal : NORMAL0;
	float4 binormal : BINORMAL0;
	float4 tangent : TANGENT0;
	float4 color : COLOR0;
	float4 tex : TEXCOORD0;
	int4	inds	: BLENDINDICES0;
	float4	weights : BLENDWEIGHT0;
};


struct GS_IN
{
	float4 pos : POSITION;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
    uint arrInd : SV_RenderTargetArrayIndex;
};

StructuredBuffer<float4x4> AnimFrame : register(t0);

#ifndef SKINNED_MESH

GS_IN VSMain(VS_IN input)
{
	GS_IN output = (GS_IN) 0;
	
	output.pos = mul(float4(input.pos.xyz, 1.0f), ConstData.World);
	
	return output;
}

#else

GS_IN VSMain(VS_IN_ANIM input)
{
	GS_IN output = (GS_IN) 0;

	float3 animPos = 0;
	float3 animNorm = 0;
	float3 animBi = 0;
	float3 animTan = 0;

	[unroll]
	for (int i = 0; i < 4; ++i) {
		float boneInd	= input.inds[i];
		float weight	= input.weights[i];

		float4x4 frameMat = AnimFrame[boneInd];

		animPos += mul(float4(input.pos.xyz, 1.0f), frameMat).xyz * weight;
	}

	output.pos = mul(float4(animPos, 1.0f), ConstData.World);
    
	return output;
}

#endif

[instance(4)]
[maxvertexcount(3)]
void GSMain(triangle GS_IN p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; ++i) {
        GS_OUT gs = (GS_OUT)0;
        
        gs.pos      = mul(float4(p[i].pos.xyz, 1.0f), CascData.ViewProj[id]);
        gs.arrInd   = id;
        stream.Append(gs);    
    }
}
