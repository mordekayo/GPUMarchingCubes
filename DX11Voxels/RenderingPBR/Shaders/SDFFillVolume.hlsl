

struct ConstParams
{
	float4 SizeXYZScale;
	float4 PositionIsoline;
	float4 Offset;
	float4 CornerPosWorldSize; // xyz - World Corner Position, w - world size
};


cbuffer CB1 : register(b0)
{
	ConstParams Params;
};

RWTexture3D<float> volumeTex : register(u0);


float sdSphere(float3 p, float3 spherePos, float s)
{
	return length(p - spherePos) - s;
}


[numthreads(8, 8, 8)]
void SdfFillVolume(in uint3 coords : SV_DispatchThreadID)
{
    if (coords.x >= Params.SizeXYZScale.x + 1)
        return;
    if (coords.y >= Params.SizeXYZScale.y + 1)
        return;
    if (coords.z >= Params.SizeXYZScale.z + 1)
        return;

	int4 volCoords = int4(coords, 0);
	float3 texCoords = (float3) coords / (Params.SizeXYZScale.xyz - float3(1, 1, 1));

	float3 worldCoords = Params.CornerPosWorldSize.xyz + texCoords * Params.CornerPosWorldSize.w;
	//worldCoords *= Params.SizeXYZScale.w;

	float oldVal = volumeTex[volCoords.xyz];

	if (Params.PositionIsoline.w == 1.0f)
		volumeTex[volCoords.xyz] = max(oldVal, -sdSphere(worldCoords, Params.Offset.xyz, Params.Offset.w));
	else
		volumeTex[volCoords.xyz] = min(oldVal, sdSphere(worldCoords, Params.Offset.xyz, Params.Offset.w));
}

