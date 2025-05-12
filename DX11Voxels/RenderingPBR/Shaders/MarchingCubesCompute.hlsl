#include "MarchingCubesInc.hlsl"


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


struct Triangle
{
	float3 p0;
	float3 p1;
	float3 p2;
	float3 n0;
	float3 n1;
	float3 n2;
    float3 color;
};

Texture3D<float>					volData		: register(t0);
AppendStructuredBuffer<Triangle>	triangles	: register(u0);
RWBuffer<uint> indirect : register(u1);

//RWStructuredBuffer<float>			triCounter : register(u0);

SamplerState TrilinearClamp : register(s0);


//[numthreads(1, 1, 1)]
//void FillIndirect()
//{
//	uint structCount = triCounter.IncrementCounter();
//	//uint stride = 0;
//	//trianglesWithDimensions.GetDimensions(structCount, stride);
//	indirect[0] = structCount;
//	indirect[1] = 1;
//	indirect[2] = 0;
//	indirect[3] = 0;
//}


float3 interpolateVerts(float3 v1, float3 v2, float v1Val, float v2Val, float isoline)
{
	float t = (isoline - v1Val) / (v2Val - v1Val);
	return v1.xyz + t * (v2.xyz - v1.xyz);
}


float3 CalculateNormal(float3 uvw)
{
	float d = 1.0 / (float) Params.SizeXYZScale.x;
	float3 grad;
	grad.x = volData.SampleLevel(TrilinearClamp, uvw + float3(-d, 0, 0), 0) - volData.SampleLevel(TrilinearClamp, uvw + float3(d, 0, 0), 0);
	grad.y = volData.SampleLevel(TrilinearClamp, uvw + float3(0, -d, 0), 0) - volData.SampleLevel(TrilinearClamp, uvw + float3(0, d, 0), 0);
	grad.z = volData.SampleLevel(TrilinearClamp, uvw + float3(0, 0, -d), 0) - volData.SampleLevel(TrilinearClamp, uvw + float3(0, 0, d), 0);
	return -normalize(grad);
}

float3 PosToTexCoords(float3 pos)
{
	return pos / (Params.SizeXYZScale.xyz + float3(-1, -1, -1));
}


[numthreads(8, 8, 8)]
void GenerateMarchingCubes(in uint3 coords : SV_DispatchThreadID)
{
	if (coords.x >= Params.SizeXYZScale.x-1)
		return;
	if (coords.y >= Params.SizeXYZScale.y-1)
		return;
	if (coords.z >= Params.SizeXYZScale.z-1)
		return;

	uint3 cornersPositions[8] = {
		coords + uint3(0, 0, 0),
		coords + uint3(1, 0, 0),
		coords + uint3(1, 0, 1),
		coords + uint3(0, 0, 1),
		coords + uint3(0, 1, 0),
		coords + uint3(1, 1, 0),
		coords + uint3(1, 1, 1),
		coords + uint3(0, 1, 1)
	};

	float cornersData[8] = {
		volData[cornersPositions[0]],
		volData[cornersPositions[1]],
		volData[cornersPositions[2]],
		volData[cornersPositions[3]],
		volData[cornersPositions[4]],
		volData[cornersPositions[5]],
		volData[cornersPositions[6]],
		volData[cornersPositions[7]],
	};



    float isoline = 0;//Params.PositionIsoline.w;
	int cubeIndex = 0;
	if (cornersData[0] < isoline) cubeIndex |= 1;
	if (cornersData[1] < isoline) cubeIndex |= 2;
	if (cornersData[2] < isoline) cubeIndex |= 4;
	if (cornersData[3] < isoline) cubeIndex |= 8;
	if (cornersData[4] < isoline) cubeIndex |= 16;
	if (cornersData[5] < isoline) cubeIndex |= 32;
	if (cornersData[6] < isoline) cubeIndex |= 64;
	if (cornersData[7] < isoline) cubeIndex |= 128;

	[loop]
	for (int i = 0; triTable[cubeIndex][i] != -1; i += 3)
	{
        // Get indices of corner points A and B for each of the three edges
        // of the cube that need to be joined to form the triangle.
		int a0 = cornerIndexAFromEdge[triTable[cubeIndex][i]];
		int b0 = cornerIndexBFromEdge[triTable[cubeIndex][i]];

		int a1 = cornerIndexAFromEdge[triTable[cubeIndex][i + 1]];
		int b1 = cornerIndexBFromEdge[triTable[cubeIndex][i + 1]];

		int a2 = cornerIndexAFromEdge[triTable[cubeIndex][i + 2]];
		int b2 = cornerIndexBFromEdge[triTable[cubeIndex][i + 2]];

		Triangle tri;
		tri.p0 = PosToTexCoords(interpolateVerts(cornersPositions[a1], cornersPositions[b1], cornersData[a1], cornersData[b1], isoline));
		tri.p1 = PosToTexCoords(interpolateVerts(cornersPositions[a0], cornersPositions[b0], cornersData[a0], cornersData[b0], isoline));
		tri.p2 = PosToTexCoords(interpolateVerts(cornersPositions[a2], cornersPositions[b2], cornersData[a2], cornersData[b2], isoline));

		float3 normal = normalize(cross(tri.p2 - tri.p0, tri.p1 - tri.p0));

		tri.n0 = normal;
		tri.n1 = normal;
		tri.n2 = normal;
		
        float3 colors[9] = { { 0.8f, 0.0f, 0.0f }, { 0.5f, 0.2f, 0.2f }, { 0.3f, 0.3f, 0.3f }, { 0.3f, 0.8f, 0.2f }, { 0.6f, 0.9f, 0.1f }, { 0.1f, 0.3f, 0.7f }, { 0.4f, 0.9f, 0.2f }, { 0.1f, 0.1f, 1.0f }, { 1.0f, 0.2f, 0.2f } };
		
        tri.color = colors[Params.PositionIsoline.w];

		//tri.n0 = CalculateNormal(tri.p0);
		//tri.n1 = CalculateNormal(tri.p1);
		//tri.n2 = CalculateNormal(tri.p2);

		//tri.p0 = lerp(cornersPositions[a0], cornersPositions[b0], 0.5f);
		//tri.p1 = lerp(cornersPositions[a1], cornersPositions[b1], 0.5f);
		//tri.p2 = lerp(cornersPositions[a2], cornersPositions[b2], 0.5f);

		triangles.Append(tri);
		//triCounter.IncrementCounter();
		//triCounter.IncrementCounter();
		//triCounter.IncrementCounter();
		InterlockedAdd(indirect[0], 3);

	}
}
