

struct VSOutput
{
	int vertexID : TEXCOORD0;
};

struct GSOutput
{
	float4 Position : SV_Position;
	float4 Color	: COLOR0;
	float3 Tex		: TEXCOORD0;
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



VSOutput VSMain(uint vertexID : SV_VertexID)
{
	VSOutput output;
	output.vertexID = vertexID;
	return output;
}


Texture3D<float> volumeTex : register(t0);

[maxvertexcount(4)]
void GSMain(point VSOutput inputPoint[1], inout TriangleStream<GSOutput> outputStream)
{
	GSOutput p0, p1, p2, p3;

	uint id = inputPoint[0].vertexID;

	int size	= (int)Params.PositionSize.w;
	int slice	= id % (size * size);
	int4 volCoords = int4(slice%size, slice/size, id/(size*size), 0);

	float val = volumeTex.Load(volCoords);

	if (val > Params.ScaleParticleSizeThreshold.z) {
		return;
	}

	float sz = Params.ScaleParticleSizeThreshold.y;
	float3 texCoord = float3(volCoords.xyz) / (float) (size - 1);


	float4 color = lerp(float4(1.0f, 0.0f, 0.0f, 1.0f), float4(0.0f, 1.0f, 0.0f, 1.0f), (val+1.0f)*0.5f);
	color = float4(texCoord, 1.0f);

	float4 wvPos = float4(Params.PositionSize.xyz + texCoord * Params.ScaleParticleSizeThreshold.x, 1.0f);
	wvPos = mul(float4(wvPos.xyz, 1.0f), Params.World);
	wvPos = mul(float4(wvPos.xyz, 1.0f), Params.View);
	wvPos = float4(wvPos.xyz, 1.0f);


	p0.Position = mul(wvPos + float4(sz, sz, 0, 0), Params.Projection);
	p0.Tex = float3(1, 1, id);
	p0.Color = color;

	p1.Position = mul(wvPos + float4(-sz, sz, 0, 0), Params.Projection);
	p1.Tex = float3(0, 1, id);
	p1.Color = color;

	p2.Position = mul(wvPos + float4(-sz, -sz, 0, 0), Params.Projection);
	p2.Tex = float3(0, 0, id);
	p2.Color = color;

	p3.Position = mul(wvPos + float4(sz, -sz, 0, 0), Params.Projection);
	p3.Tex = float3(1, 0, id);
	p3.Color = color;

	outputStream.Append(p1);
	outputStream.Append(p0);
	outputStream.Append(p2);
	outputStream.Append(p3);
}



float4 PSMain(GSOutput input) : SV_Target0
{

	float amount = length(input.Tex - float2(0.5f, 0.5f)) * 2.0f;

	amount = smoothstep(0.0f, 1.0f, 1.0f - amount);

	clip(amount - 0.2f);

	return float4(input.Color.rgb, 1.0f);
}
