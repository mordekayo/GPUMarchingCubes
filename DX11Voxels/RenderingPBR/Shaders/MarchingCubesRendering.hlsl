
struct VSOutput
{
	float4 Position : SV_Position;
	float4 WorldPos : WORLDPOS0;
	float4 Normal : NORMAL0;
	float4 Color	: COLOR0;
};


struct ConstParams
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4 PositionSize;
	float4 ScaleParticleSizeThreshold;
};


cbuffer CB1 : register(b0)
{
	ConstParams Params;
};

struct Triangle
{
	float3 pos[3];
	float3 norm[3];
    float3 color;
};

StructuredBuffer<Triangle> triangles : register(t0);



VSOutput VSMain(uint vertexID : SV_VertexID)
{
	Triangle tri = triangles[vertexID / 3];

	uint ind = vertexID % 3;

	float3 position = tri.pos[ind] * Params.ScaleParticleSizeThreshold.x + Params.PositionSize.xyz;

	VSOutput output;
	output.WorldPos = mul(float4(position, 1.0f), Params.World);
	float4 viewPos	= mul(float4(output.WorldPos.xyz, 1.0f), Params.View);

	output.WorldPos.w = abs(viewPos.z / viewPos.w);

	output.Position = mul(float4(viewPos.xyz, 1.0f), Params.Projection);
    output.Color	= float4(tri.color, 1.0f);
	output.Normal	= float4(tri.norm[ind], 0.0f);
	return output;
}


VSOutput VSMainBuf(in float3 position : POSITION0, in float3 normal : NORMAL0)
{
	position *= Params.ScaleParticleSizeThreshold.x;
	
	VSOutput output;
	output.WorldPos = mul(float4(position, 1.0f), Params.World);
	float4 viewPos = mul(float4(output.WorldPos.xyz, 1.0f), Params.View);
	
	output.WorldPos.w = abs(viewPos.z / viewPos.w);
	
	output.Position = mul(float4(viewPos.xyz, 1.0f), Params.Projection);
	output.Color = float4(0.8f, 0.8f, 0.8f, 1.0f);
	output.Normal = float4(normal, 0.0f);
	return output;
}


struct PSOutput
{
	float4 Diffuse		: SV_Target0; // a - Specular
	float4 Normal		: SV_Target1;
	float4 MetRougAoId	: SV_Target2;
	float4 Emissive		: SV_Target3; // Emissive Color
	float4 WorldPos		: SV_Target4;
};


PSOutput PSMain(VSOutput input) : SV_Target0
{
	PSOutput ret;
	ret.Diffuse = input.Color;
	ret.Normal	= float4(normalize(input.Normal.xyz), 0.0f);
	ret.Emissive = float4(1, 1, 1, 0);
	ret.WorldPos = input.WorldPos;
	ret.MetRougAoId = float4(1, 1, 1, 0);

	return ret;
}




struct GS_IN
{
	float4 pos : POSITION;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	uint arrInd : SV_RenderTargetArrayIndex;
};


struct CascadeData
{
	float4x4 ViewProj[4];
	float4 Distances;
};

cbuffer CascBuf : register(b0)
{
	CascadeData CascData;
}

cbuffer WorldBuf : register(b1)
{
	ConstParams WorldData;
}


GS_IN VSRenderToShadowMap(uint vertexID : SV_VertexID)
{
	Triangle tri = triangles[vertexID / 3];

	uint ind = vertexID % 3;

	float3 position = tri.pos[ind] + WorldData.PositionSize.xyz;
	position *= WorldData.ScaleParticleSizeThreshold.x;

	GS_IN output;
	output.pos = mul(float4(position, 1.0f), WorldData.World);
	return output;
}


[instance(4)]
[maxvertexcount(3)]
void GSRenderToShadowMap(triangle GS_IN p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
	for (int i = 0; i < 3; ++i)
	{
		GS_OUT gs = (GS_OUT) 0;
        
		gs.pos = mul(float4(p[i].pos.xyz, 1.0f), CascData.ViewProj[id]);
		gs.arrInd = id;
		stream.Append(gs);
	}
}