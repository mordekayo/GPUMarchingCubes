#include "MarchingCubesInc.hlsl"


struct ConstParams
{
	float4 SizeXYZScale;
	float4 PositionIsoline;
	float4 Offset;
	float4 CornerPosWorldSize; // xyz - World Corner Position, w - world size
	float4 TransitionParams;
	float4 TransitionParams2;
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


Texture3D<float>					volData					: register(t0);
Texture2D<int>					    triTable		        : register(t1);
Texture2D<int>						transitionLookUpTable13	: register(t2);
Texture2D<int>						transitionLookUpTable15	: register(t3);
Texture2D<int>						transitionLookUpTable14	: register(t4);
AppendStructuredBuffer<Triangle>	triangles				: register(u0);
RWBuffer<uint>						indirect				: register(u1);

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
    if(t < 0)
    {
        t = 0;
    }
    if(t > 1)
    {
        t = 1;
    }
    return v1.xyz + 0.5f * (v2.xyz - v1.xyz);
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

void ComputeDefaultNode(uint3 coords)
{
    uint3 cornersPositions[8] =
    {
        coords + uint3(0, 0, 0),
		coords + uint3(1, 0, 0),
		coords + uint3(1, 0, 1),
		coords + uint3(0, 0, 1),
		coords + uint3(0, 1, 0),
		coords + uint3(1, 1, 0),
		coords + uint3(1, 1, 1),
		coords + uint3(0, 1, 1)
    };

    float cornersData[8] =
    {
        volData[cornersPositions[0]],
		volData[cornersPositions[1]],
		volData[cornersPositions[2]],
		volData[cornersPositions[3]],
		volData[cornersPositions[4]],
		volData[cornersPositions[5]],
		volData[cornersPositions[6]],
		volData[cornersPositions[7]]
    };

    float isoline = 0;
    int cubeIndex = 0;
    if (cornersData[0] < isoline)
        cubeIndex |= 1;
    if (cornersData[1] < isoline)
        cubeIndex |= 2;
    if (cornersData[2] < isoline)
        cubeIndex |= 4;
    if (cornersData[3] < isoline)
        cubeIndex |= 8;
    if (cornersData[4] < isoline)
        cubeIndex |= 16;
    if (cornersData[5] < isoline)
        cubeIndex |= 32;
    if (cornersData[6] < isoline)
        cubeIndex |= 64;
    if (cornersData[7] < isoline)
        cubeIndex |= 128;
        
		    [loop]
    for (int i = 0; i < 16; i += 3)
    {
        if (triTable[uint2(i, cubeIndex)] == -1)
        {
            break;
        }
        // Get indices of corner points A and B for each of the three edges
        // of the cube that need to be joined to form the triangle.
        int a0 = cornerIndexAFromEdge[triTable[uint2(i, cubeIndex)]];
        int b0 = cornerIndexBFromEdge[triTable[uint2(i, cubeIndex)]];

        int a1 = cornerIndexAFromEdge[triTable[uint2(i + 1, cubeIndex)]];
        int b1 = cornerIndexBFromEdge[triTable[uint2(i + 1, cubeIndex)]];

        int a2 = cornerIndexAFromEdge[triTable[uint2(i + 2, cubeIndex)]];
        int b2 = cornerIndexBFromEdge[triTable[uint2(i + 2, cubeIndex)]];

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

        triangles.Append(tri);
        InterlockedAdd(indirect[0], 3);
    }
}

void ComputeTransitionNode14(uint3 coords, int side14)
{
    bool correctCellIndex = true;

    //switch (Params.TransitionParams2.z)
    //{
    //    case 0:
    //    {
    //            correctCellIndex = coords.x % 2 == 1 && coords.y % 2 == 1 && coords.z % 2 == 0;
    //            break;
    //        }
    //    case 1:
    //    {
    //            correctCellIndex = coords.x % 2 == 1 && coords.y % 2 == 1 && coords.z % 2 == 1;
    //            break;
    //        }
    //    case 2:
    //    {
    //            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 1 && coords.z % 2 == 1;
    //            break;
    //        }
    //    case 3:
    //    {
    //            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 1 && coords.z % 2 == 0;
    //            break;
    //        }
    //    case 4:
    //    {
    //            correctCellIndex = coords.x % 2 == 1 && coords.y % 2 == 0 && coords.z % 2 == 0;
    //            break;
    //        }
    //    case 5:
    //    {
    //            correctCellIndex = coords.x % 2 == 1 && coords.y % 2 == 0 && coords.z % 2 == 1;
    //            break;
    //        }
    //    case 6:
    //    {
    //            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 0 && coords.z % 2 == 1;
    //            break;
    //        }
    //    case 7:
    //    {
    //            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 0 && coords.z % 2 == 0;
    //            break;
    //        }
    //}

    if (!correctCellIndex)
    {
        return;
    }
    
    uint3 TcornersPositions[27] =
    {
        coords + uint3(0, 0, 0), //0
		coords + uint3(1, 0, 0), //1
		coords + uint3(1, 0, 1), //2
		coords + uint3(0, 0, 1), //3
		coords + uint3(0, 1, 0), //4
		coords + uint3(1, 1, 0), //5
		coords + uint3(1, 1, 1), //6
		coords + uint3(0, 1, 1), //7
        coords + uint3(2, 0, 0), //8
		coords + uint3(2, 0, 1), //9
		coords + uint3(2, 1, 0), //10
		coords + uint3(2, 1, 1), //11
        coords + uint3(1, 0, 2), //12
		coords + uint3(0, 0, 2), //13
		coords + uint3(1, 1, 2), //14
		coords + uint3(0, 1, 2), //15
		coords + uint3(2, 0, 2), //16
		coords + uint3(2, 1, 2), //17
        coords + uint3(0, 2, 0), //18
		coords + uint3(1, 2, 0), //19
		coords + uint3(1, 2, 1), //20
		coords + uint3(0, 2, 1), //21
		coords + uint3(2, 2, 0), //22
		coords + uint3(2, 2, 1), //23
        coords + uint3(1, 2, 2), //24
		coords + uint3(0, 2, 2), //25
        coords + uint3(2, 2, 2) //26
    };
    
    float TcornersData[27] =
    {
        volData[TcornersPositions[transCorners14[side14][0]]],
		volData[TcornersPositions[transCorners14[side14][1]]],
		volData[TcornersPositions[transCorners14[side14][2]]],
		volData[TcornersPositions[transCorners14[side14][3]]],
		volData[TcornersPositions[transCorners14[side14][4]]],
		volData[TcornersPositions[transCorners14[side14][5]]],
		volData[TcornersPositions[transCorners14[side14][6]]],
		volData[TcornersPositions[transCorners14[side14][7]]],
		volData[TcornersPositions[transCorners14[side14][8]]],
		volData[TcornersPositions[transCorners14[side14][9]]],
		volData[TcornersPositions[transCorners14[side14][10]]],
		volData[TcornersPositions[transCorners14[side14][11]]],
		volData[TcornersPositions[transCorners14[side14][12]]],
		volData[TcornersPositions[transCorners14[side14][13]]],
		volData[TcornersPositions[transCorners14[side14][14]]],
		volData[TcornersPositions[transCorners14[side14][15]]],
		volData[TcornersPositions[transCorners14[side14][16]]],
		volData[TcornersPositions[transCorners14[side14][17]]],
		volData[TcornersPositions[transCorners14[side14][18]]],
		volData[TcornersPositions[transCorners14[side14][19]]],
		volData[TcornersPositions[transCorners14[side14][20]]],
		volData[TcornersPositions[transCorners14[side14][21]]],
		volData[TcornersPositions[transCorners14[side14][22]]],
		volData[TcornersPositions[transCorners14[side14][23]]],
		volData[TcornersPositions[transCorners14[side14][24]]],
		volData[TcornersPositions[transCorners14[side14][25]]],
		volData[TcornersPositions[transCorners14[side14][26]]]
    };
	
    float Tisoline = 0; //Params.PositionIsoline.w;
    int TcubeIndex = 0;
    if (TcornersData[0] < Tisoline)
        TcubeIndex |= 1;
    if (TcornersData[8] < Tisoline)
        TcubeIndex |= 2;
    if (TcornersData[16] < Tisoline)
        TcubeIndex |= 4;
    if (TcornersData[13] < Tisoline)
        TcubeIndex |= 8;
    if (TcornersData[6] < Tisoline)
        TcubeIndex |= 16;
    if (TcornersData[11] < Tisoline)
        TcubeIndex |= 32;
    if (TcornersData[17] < Tisoline)
        TcubeIndex |= 64;
    if (TcornersData[14] < Tisoline)
        TcubeIndex |= 128;
    if (TcornersData[18] < Tisoline)
        TcubeIndex |= 256;
    if (TcornersData[22] < Tisoline)
        TcubeIndex |= 512;
    if (TcornersData[23] < Tisoline)
        TcubeIndex |= 1024;
    if (TcornersData[20] < Tisoline)
        TcubeIndex |= 2048;
    if (TcornersData[24] < Tisoline)
        TcubeIndex |= 4096;
    if (TcornersData[25] < Tisoline)
        TcubeIndex |= 8192;
    
    if (TcubeIndex > 8191)
    {
        TcubeIndex = 16383 - TcubeIndex;
    }
    
	[loop]
    for (int i = 0; i < 75; i += 3)
    {
        //Get indices of corner points A and B for each of the three edges
        //of the cube that need to be joined to form the triangle.
        if (transitionLookUpTable14[uint2(i, TcubeIndex)] == -1)
        {
            break;
        }
        int a0 = transCornerIndexAFromEdge[transitionLookUpTable14[uint2(i, TcubeIndex)]];
        int b0 = transCornerIndexBFromEdge[transitionLookUpTable14[uint2(i, TcubeIndex)]];

        int a1 = transCornerIndexAFromEdge[transitionLookUpTable14[uint2(i + 1, TcubeIndex)]];
        int b1 = transCornerIndexBFromEdge[transitionLookUpTable14[uint2(i + 1, TcubeIndex)]];

        int a2 = transCornerIndexAFromEdge[transitionLookUpTable14[uint2(i + 2, TcubeIndex)]];
        int b2 = transCornerIndexBFromEdge[transitionLookUpTable14[uint2(i + 2, TcubeIndex)]];
        
        Triangle tri;
        
        tri.p0 = PosToTexCoords(interpolateVerts(TcornersPositions[transCorners14[side14][a1]], TcornersPositions[transCorners14[side14][b1]], TcornersData[transCorners14[0][a1]], TcornersData[transCorners14[0][b1]], Tisoline));
        tri.p1 = PosToTexCoords(interpolateVerts(TcornersPositions[transCorners14[side14][a0]], TcornersPositions[transCorners14[side14][b0]], TcornersData[transCorners14[0][a0]], TcornersData[transCorners14[0][b0]], Tisoline));
        tri.p2 = PosToTexCoords(interpolateVerts(TcornersPositions[transCorners14[side14][a2]], TcornersPositions[transCorners14[side14][b2]], TcornersData[transCorners14[0][a2]], TcornersData[transCorners14[0][b2]], Tisoline));
        
        float3 normal1 = CalculateNormal(tri.p0);
        float3 normal2 = CalculateNormal(tri.p1);
        float3 normal3 = CalculateNormal(tri.p2);
        
        float3 meanTriangleNormal = float3((normal1.x + normal2.x + normal3.x) / 3,
                                   (normal1.y + normal2.y + normal3.y) / 3,
                                   (normal1.z + normal2.z + normal3.z) / 3);
        
        float3 normal = normalize(cross(tri.p2 - tri.p0, tri.p1 - tri.p0));
        
        if (dot(normal, meanTriangleNormal) <= 0)
        {
            float3 temp = tri.p0;
            tri.p0 = tri.p2;
            tri.p2 = temp;
            normal = -normal;
        }
        
        tri.n0 = normal;
        tri.n1 = normal;
        tri.n2 = normal;

        tri.color = float3(1.0f, 1.0f, 1.0f);
        
        triangles.Append(tri);
        InterlockedAdd(indirect[0], 3);
    }
}

void ComputeTransitionNode15(uint3 coords, int side15)
{
    //return;
    bool correctCellIndex = true;

    switch (Params.TransitionParams2.z)
    {
        case 0:
        {
            correctCellIndex = coords.x % 2 == 1 && coords.y % 2 == 1 && coords.z % 2 == 0;
            break;
        }
        case 1:
        {
            correctCellIndex = coords.x % 2 == 1 && coords.y % 2 == 1 && coords.z % 2 == 1;
            break;
        }
        case 2:
        {
            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 1 && coords.z % 2 == 1;
            break;
        }
        case 3:
        {
            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 1 && coords.z % 2 == 0;
            break;
        }
        case 4:
        {
            correctCellIndex = coords.x % 2 == 1 && coords.y % 2 == 0 && coords.z % 2 == 0;
            break;
        }
        case 5:
        {
            correctCellIndex = coords.x % 2 == 1 && coords.y % 2 == 0 && coords.z % 2 == 1;
            break;
        }
        case 6:
        {
            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 0 && coords.z % 2 == 1;
            break;
        }
        case 7:
        {
            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 0 && coords.z % 2 == 0;
            break;
        }
    }

    if (!correctCellIndex)
    {
        return;
    }
    
    uint3 TcornersPositions[27] =
    {
        coords + uint3(0, 0, 0), //0
		coords + uint3(1, 0, 0), //1
		coords + uint3(1, 0, 1), //2
		coords + uint3(0, 0, 1), //3
		coords + uint3(0, 1, 0), //4
		coords + uint3(1, 1, 0), //5
		coords + uint3(1, 1, 1), //6
		coords + uint3(0, 1, 1), //7
        coords + uint3(2, 0, 0), //8
		coords + uint3(2, 0, 1), //9
		coords + uint3(2, 1, 0), //10
		coords + uint3(2, 1, 1), //11
        coords + uint3(1, 0, 2), //12
		coords + uint3(0, 0, 2), //13
		coords + uint3(1, 1, 2), //14
		coords + uint3(0, 1, 2), //15
		coords + uint3(2, 0, 2), //16
		coords + uint3(2, 1, 2), //17
        coords + uint3(0, 2, 0), //18
		coords + uint3(1, 2, 0), //19
		coords + uint3(1, 2, 1), //20
		coords + uint3(0, 2, 1), //21
		coords + uint3(2, 2, 0), //22
		coords + uint3(2, 2, 1), //23
        coords + uint3(1, 2, 2), //24
		coords + uint3(0, 2, 2), //25
        coords + uint3(2, 2, 2)  //26
    };
    
    float TcornersData[27] =
    {
        volData[TcornersPositions[transCorners15[side15][0]]],
		volData[TcornersPositions[transCorners15[side15][1]]],
		volData[TcornersPositions[transCorners15[side15][2]]],
		volData[TcornersPositions[transCorners15[side15][3]]],
		volData[TcornersPositions[transCorners15[side15][4]]],
		volData[TcornersPositions[transCorners15[side15][5]]],
		volData[TcornersPositions[transCorners15[side15][6]]],
		volData[TcornersPositions[transCorners15[side15][7]]],
		volData[TcornersPositions[transCorners15[side15][8]]],
		volData[TcornersPositions[transCorners15[side15][9]]],
		volData[TcornersPositions[transCorners15[side15][10]]],
		volData[TcornersPositions[transCorners15[side15][11]]],
		volData[TcornersPositions[transCorners15[side15][12]]],
		volData[TcornersPositions[transCorners15[side15][13]]],
		volData[TcornersPositions[transCorners15[side15][14]]],
		volData[TcornersPositions[transCorners15[side15][15]]],
		volData[TcornersPositions[transCorners15[side15][16]]],
		volData[TcornersPositions[transCorners15[side15][17]]],
		volData[TcornersPositions[transCorners15[side15][18]]],
		volData[TcornersPositions[transCorners15[side15][19]]],
		volData[TcornersPositions[transCorners15[side15][20]]],
		volData[TcornersPositions[transCorners15[side15][21]]],
		volData[TcornersPositions[transCorners15[side15][22]]],
		volData[TcornersPositions[transCorners15[side15][23]]],
		volData[TcornersPositions[transCorners15[side15][24]]],
		volData[TcornersPositions[transCorners15[side15][25]]],
		volData[TcornersPositions[transCorners15[side15][26]]]
    };
	
    float Tisoline = 0; //Params.PositionIsoline.w;
    int TcubeIndex = 0;
    if (TcornersData[0] < Tisoline)
        TcubeIndex |= 1;
    if (TcornersData[8] < Tisoline)
        TcubeIndex |= 2;
    if (TcornersData[9] < Tisoline)
        TcubeIndex |= 4;
    if (TcornersData[2] < Tisoline)
        TcubeIndex |= 8;
    if (TcornersData[12] < Tisoline)
        TcubeIndex |= 16;
    if (TcornersData[13] < Tisoline)
        TcubeIndex |= 32;
    if (TcornersData[11] < Tisoline)
        TcubeIndex |= 64;
    if (TcornersData[6] < Tisoline)
        TcubeIndex |= 128;
    if (TcornersData[14] < Tisoline)
        TcubeIndex |= 256;
    if (TcornersData[18] < Tisoline)
        TcubeIndex |= 512;
    if (TcornersData[22] < Tisoline)
        TcubeIndex |= 1024;
    if (TcornersData[23] < Tisoline)
        TcubeIndex |= 2048;
    if (TcornersData[20] < Tisoline)
        TcubeIndex |= 4096;
    if (TcornersData[24] < Tisoline)
        TcubeIndex |= 8192;
    if (TcornersData[25] < Tisoline)
        TcubeIndex |= 16384;
    
    if (TcubeIndex > 16384)
    {
        TcubeIndex = 32767 - TcubeIndex;
    }
    
	[loop]
    for (int i = 0; i < 120; i += 3)
    {
        //Get indices of corner points A and B for each of the three edges
        //of the cube that need to be joined to form the triangle.
        if (transitionLookUpTable15[uint2(i, TcubeIndex)] == -1)
        {
            break;
        }
        int a0 = transCornerIndexAFromEdge[transitionLookUpTable15[uint2(i, TcubeIndex)]];
        int b0 = transCornerIndexBFromEdge[transitionLookUpTable15[uint2(i, TcubeIndex)]];

        int a1 = transCornerIndexAFromEdge[transitionLookUpTable15[uint2(i + 1, TcubeIndex)]];
        int b1 = transCornerIndexBFromEdge[transitionLookUpTable15[uint2(i + 1, TcubeIndex)]];

        int a2 = transCornerIndexAFromEdge[transitionLookUpTable15[uint2(i + 2, TcubeIndex)]];
        int b2 = transCornerIndexBFromEdge[transitionLookUpTable15[uint2(i + 2, TcubeIndex)]];
        
        Triangle tri;
        
        tri.p0 = PosToTexCoords(interpolateVerts(TcornersPositions[transCorners15[side15][a1]], TcornersPositions[transCorners15[side15][b1]], TcornersData[transCorners15[0][a1]], TcornersData[transCorners15[0][b1]], Tisoline));
        tri.p1 = PosToTexCoords(interpolateVerts(TcornersPositions[transCorners15[side15][a0]], TcornersPositions[transCorners15[side15][b0]], TcornersData[transCorners15[0][a0]], TcornersData[transCorners15[0][b0]], Tisoline));
        tri.p2 = PosToTexCoords(interpolateVerts(TcornersPositions[transCorners15[side15][a2]], TcornersPositions[transCorners15[side15][b2]], TcornersData[transCorners15[0][a2]], TcornersData[transCorners15[0][b2]], Tisoline));
        
        float3 normal1 = CalculateNormal(tri.p0);
        float3 normal2 = CalculateNormal(tri.p1);
        float3 normal3 = CalculateNormal(tri.p2);
        
        float3 meanTriangleNormal = float3((normal1.x + normal2.x + normal3.x) / 3,
                                   (normal1.y + normal2.y + normal3.y) / 3,
                                   (normal1.z + normal2.z + normal3.z) / 3);
        
        float3 normal = normalize(cross(tri.p2 - tri.p0, tri.p1 - tri.p0));
        
        if (dot(normal, meanTriangleNormal) <= 0)
        {
            float3 temp = tri.p0;
            tri.p0 = tri.p2;
            tri.p2 = temp;
            normal = -normal;
        }
        
        tri.n0 = normal;
        tri.n1 = normal;
        tri.n2 = normal;

        tri.color = float3(1.0f, 0.55f, 0.0f);
        
        triangles.Append(tri);
        InterlockedAdd(indirect[0], 3);
    }
}

void ComputeTransitionNode13(uint3 coords, uint side)
{   
    switch (side)
    {
        case 0:
        {
            if (coords.x < Params.TransitionParams.x && coords.x == 0)
            {
                if (Params.TransitionParams.z > 0 && coords.y == 0)
                {
                    ComputeTransitionNode14(coords, 0);//FLD
                }
                else if (Params.TransitionParams.w > 0 && coords.y == Params.SizeXYZScale.y - 3)
                {
                    ComputeTransitionNode14(coords, 1);//FLU
                }
                else
                {
                    ComputeTransitionNode15(coords, 0);//FL
                }
                return;
            }
            if (Params.TransitionParams.y > 0 && coords.x == Params.SizeXYZScale.x - 3)
            {
                 if (Params.TransitionParams.z > 0 && coords.y == 0)
                {
                    ComputeTransitionNode14(coords, 2);//FRD
                }
                else if (Params.TransitionParams.w > 0)
                {
                    ComputeTransitionNode14(coords, 3);//FRU
                }
                else
                {
                    ComputeTransitionNode15(coords, 1); //FR
                }
                return;
            }
            if (Params.TransitionParams.z > 0 && coords.y == 0)
            {
                ComputeTransitionNode15(coords, 4);//FD
                return;
            }
            if (Params.TransitionParams.w > 0 && coords.y == Params.SizeXYZScale.x - 3)
            {
                ComputeTransitionNode15(coords, 5);//FU   
                return;
            }
            if (coords.z >= Params.SizeXYZScale.z - 2 - Params.TransitionParams2.y)
            {
                ComputeDefaultNode(coords);
                return;
            }
            break;
        }
        case 1:
        {
            if (Params.TransitionParams2.y > 0 && coords.x == Params.SizeXYZScale.x - 3 && coords.z == Params.SizeXYZScale.z - 3)
            {
                ComputeDefaultNode(coords);
                if (Params.TransitionParams.z > 0 && coords.y == 0)
                {
                    ComputeTransitionNode14(coords, 4);//RBD
                }
                else if (Params.TransitionParams.w > 0 && coords.y == Params.SizeXYZScale.y - 3)
                {
                    ComputeTransitionNode14(coords, 5);//RBU
                }
                else
                {
                    ComputeTransitionNode15(coords, 2);//RB
                }
                return;
            }
            if (Params.TransitionParams.z > 0 && coords.y == 0)
            {
                ComputeTransitionNode15(coords, 6);
                return;
            }
            if (Params.TransitionParams.w > 0 && coords.y == Params.SizeXYZScale.y - 3)
            {
                ComputeTransitionNode15(coords, 7);
                return;
            }
            else if (coords.x <= Params.SizeXYZScale.y - 2 - Params.TransitionParams.y &&
                coords.z <= Params.SizeXYZScale.y - 2 - Params.TransitionParams2.y)
            {
                ComputeDefaultNode(coords);
                return;
            }
            if (Params.TransitionParams2.y > 0 && coords.z >= Params.SizeXYZScale.y - 3)
            {
                return;
            }
            break;
        }
        case 2:
        {
            if (Params.TransitionParams.x > 0 && coords.x == 0)
            {
                if (Params.TransitionParams.z > 0 && coords.y == 0)
                {
                    ComputeTransitionNode14(coords, 6); //BLD
                }
                else if (Params.TransitionParams.w > 0 && coords.y == Params.SizeXYZScale.y - 3)
                {
                    ComputeTransitionNode14(coords, 7); //BLU
                }
                else
                {
                    ComputeTransitionNode15(coords, 3); //BL
                }
                
                ComputeTransitionNode15(coords, 3); //BL

                return;
            }
            else
            if (Params.TransitionParams.z > 0 && coords.y == 0)
            {
                ComputeTransitionNode15(coords, 8);
                return;
            }
            else
            if (Params.TransitionParams.w > 0 && coords.y == Params.SizeXYZScale.y - 3)
            {
                if (coords.z == Params.SizeXYZScale.z- 3)
                {
                    ComputeDefaultNode(coords);
                }
                ComputeTransitionNode15(coords, 9);
                return;
            }
            else if (coords.z == Params.SizeXYZScale.z - 3 && (Params.TransitionParams.w == 0 || coords.y < Params.SizeXYZScale.y - 2))
            {
                ComputeDefaultNode(coords);
                return;
            }
            else if (Params.TransitionParams.x > 0 && coords.x == 0)
            {
                return;
            }
            break;
        }
        case 3:
        {
            if (Params.TransitionParams.z > 0 && coords.y == 0)
            {
                ComputeTransitionNode15(coords, 10);
                return;
            }
            if (Params.TransitionParams.w > 0 && coords.y == Params.SizeXYZScale.y - 3 && coords.x == 0)
            {
                ComputeTransitionNode15(coords, 11);
            }
            break;
        }
        case 4:
        {
            break;
        }
        case 5:
        {
                //if (Params.TransitionParams.x > 0 || Params.TransitionParams.y > 0 ||
                //Params.TransitionParams.z > 0 || Params.TransitionParams2.x > 0 || Params.TransitionParams2.y > 0)
                //{
                //    return;
                //}
                //if (coords.z == Params.SizeXYZScale.z - 3)
                //{
                //    return;
                //}
                break;
        }
    }
    
    bool correctCellIndex = true;
    
    switch (Params.TransitionParams2.z)
    {
        case 0:
        {
            if (side == 5)
            {
                 correctCellIndex = coords.x % 2 == 1 && coords.y % 2 == 0 && coords.z % 2 == 0;
            }
            else
            {
                 correctCellIndex = coords.x % 2 != side % 2 && coords.y % 2 == 1 && coords.z % 2 == 0;
            }
            break;
        }
        case 1:
        {  
            if(side == 5)
            {
                correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 0 && coords.z % 2 == 0;
            }
            else
            {
                correctCellIndex = coords.x % 2 != side % 2 && coords.y % 2 == 1 && coords.z % 2 == side % 2;
            }
            break;
        }
        case 2:
        {
            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == (side == 5 ? 0 : 1) && coords.z % 2 == side % 2;
            break;
        }
        case 3:
        {
            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == (side == 5 ? 0 : 1) && coords.z % 2 == 0;
            break;
        }
        case 4:
        {
            correctCellIndex = coords.x % 2 != side % 2 && coords.y % 2 == 0 && coords.z % 2 == 0;
            break;
        }
        case 5:
        {
            correctCellIndex = coords.x % 2 != (side == 4 ? 0 : side % 2) && coords.y % 2 == 0 && coords.z % 2 == (side == 4 ? 1 : side % 2);
            break;
        }
        case 6:
        {
            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 0 && coords.z % 2 == (side == 4 ? 1 : side % 2);
            break;
        }
        case 7:
        {
            correctCellIndex = coords.x % 2 == 0 && coords.y % 2 == 0 && coords.z % 2 == 0;
            break;
        }
    }

    if (!correctCellIndex)
    {
        return;
    }
    
    uint3 TcornersPositions[27] =
    {
        coords + uint3(0, 0, 0), //0
		coords + uint3(1, 0, 0), //1
		coords + uint3(1, 0, 1), //2
		coords + uint3(0, 0, 1), //3
		coords + uint3(0, 1, 0), //4
		coords + uint3(1, 1, 0), //5
		coords + uint3(1, 1, 1), //6
		coords + uint3(0, 1, 1), //7
        coords + uint3(2, 0, 0), //8
		coords + uint3(2, 0, 1), //9
		coords + uint3(2, 1, 0), //10
		coords + uint3(2, 1, 1), //11
        coords + uint3(1, 0, 2), //12
		coords + uint3(0, 0, 2), //13
		coords + uint3(1, 1, 2), //14
		coords + uint3(0, 1, 2), //15
		coords + uint3(2, 0, 2), //16
		coords + uint3(2, 1, 2), //17
        coords + uint3(0, 2, 0), //18
		coords + uint3(1, 2, 0), //19
		coords + uint3(1, 2, 1), //20
		coords + uint3(0, 2, 1), //21
		coords + uint3(2, 2, 0), //22
		coords + uint3(2, 2, 1), //23
        coords + uint3(1, 2, 2), //24
		coords + uint3(0, 2, 2), //25
        coords + uint3(2, 2, 2)  //26
    };
    
    float TcornersData[27] =
    {
        volData[TcornersPositions[transCorners[side][0]]],
		volData[TcornersPositions[transCorners[side][1]]],
		volData[TcornersPositions[transCorners[side][2]]],
		volData[TcornersPositions[transCorners[side][3]]],
		volData[TcornersPositions[transCorners[side][4]]],
		volData[TcornersPositions[transCorners[side][5]]],
		volData[TcornersPositions[transCorners[side][6]]],
		volData[TcornersPositions[transCorners[side][7]]],
		volData[TcornersPositions[transCorners[side][8]]],
		volData[TcornersPositions[transCorners[side][9]]],
		volData[TcornersPositions[transCorners[side][10]]],
		volData[TcornersPositions[transCorners[side][11]]],
		volData[TcornersPositions[transCorners[side][12]]],
		volData[TcornersPositions[transCorners[side][13]]],
		volData[TcornersPositions[transCorners[side][14]]],
		volData[TcornersPositions[transCorners[side][15]]],
		volData[TcornersPositions[transCorners[side][16]]],
		volData[TcornersPositions[transCorners[side][17]]],
		volData[TcornersPositions[transCorners[side][18]]],
		volData[TcornersPositions[transCorners[side][19]]],
		volData[TcornersPositions[transCorners[side][20]]],
		volData[TcornersPositions[transCorners[side][21]]],
		volData[TcornersPositions[transCorners[side][22]]],
		volData[TcornersPositions[transCorners[side][23]]],
		volData[TcornersPositions[transCorners[side][24]]],
		volData[TcornersPositions[transCorners[side][25]]],
		volData[TcornersPositions[transCorners[side][26]]]
    };
	
    float Tisoline = 0; //Params.PositionIsoline.w;
    int TcubeIndex = 0;
    if (TcornersData[0] < Tisoline)
        TcubeIndex |= 1;
    if (TcornersData[8] < Tisoline)
        TcubeIndex |= 2;
    if (TcornersData[9] < Tisoline)
        TcubeIndex |= 4;
    if (TcornersData[2] < Tisoline)
        TcubeIndex |= 8;
    if (TcornersData[3] < Tisoline)
        TcubeIndex |= 16;
    if (TcornersData[11] < Tisoline)
        TcubeIndex |= 32;
    if (TcornersData[6] < Tisoline)
        TcubeIndex |= 64;
    if (TcornersData[7] < Tisoline)
        TcubeIndex |= 128;
    if (TcornersData[22] < Tisoline)
        TcubeIndex |= 256;
    if (TcornersData[23] < Tisoline)
        TcubeIndex |= 512;
    if (TcornersData[20] < Tisoline)
        TcubeIndex |= 1024;
    if (TcornersData[21] < Tisoline)
        TcubeIndex |= 2048;
    if (TcornersData[18] < Tisoline)
        TcubeIndex |= 4096;
        
    bool flip = false;
    if(TcubeIndex > 4095)
    {
        TcubeIndex = 8191 - TcubeIndex;
        flip = true;
    }
   

	[loop]
    for (int i = 0; transitionLookUpTable13[uint2(i, TcubeIndex)] != -1; i += 3)
    {        
        int a0 = transCornerIndexAFromEdge[transitionLookUpTable13[uint2(i, TcubeIndex)]];
        int b0 = transCornerIndexBFromEdge[transitionLookUpTable13[uint2(i, TcubeIndex)]];

        int a1 = transCornerIndexAFromEdge[transitionLookUpTable13[uint2(i + 1, TcubeIndex)]];
        int b1 = transCornerIndexBFromEdge[transitionLookUpTable13[uint2(i + 1, TcubeIndex)]];

        int a2 = transCornerIndexAFromEdge[transitionLookUpTable13[uint2(i + 2, TcubeIndex)]];
        int b2 = transCornerIndexBFromEdge[transitionLookUpTable13[uint2(i + 2, TcubeIndex)]];

        Triangle tri;
        
        tri.p0 = PosToTexCoords(interpolateVerts(TcornersPositions[transCorners[side][a1]], TcornersPositions[transCorners[side][b1]], TcornersData[transCorners[0][a1]], TcornersData[transCorners[0][b1]], Tisoline));
        tri.p1 = PosToTexCoords(interpolateVerts(TcornersPositions[transCorners[side][a0]], TcornersPositions[transCorners[side][b0]], TcornersData[transCorners[0][a0]], TcornersData[transCorners[0][b0]], Tisoline));
        tri.p2 = PosToTexCoords(interpolateVerts(TcornersPositions[transCorners[side][a2]], TcornersPositions[transCorners[side][b2]], TcornersData[transCorners[0][a2]], TcornersData[transCorners[0][b2]], Tisoline));
        
        float3 normal = normalize(cross(tri.p2 - tri.p0, tri.p1 - tri.p0));
        
        if (flip)
        {
            float3 temp = tri.p0;
            tri.p0 = tri.p2;
            tri.p2 = temp;
            normal = -normal;
        }
        
        tri.n0 = normal;
        tri.n1 = normal;
        tri.n2 = normal;

        tri.color = float3(0.17f, 1.0f, 0.18f);
        
        //float3 colors[9] = { { 0.8f, 0.0f, 0.0f }, { 0.5f, 0.2f, 0.2f }, { 0.3f, 0.3f, 0.3f }, { 0.3f, 0.8f, 0.2f }, { 0.6f, 0.9f, 0.1f }, { 0.1f, 0.3f, 0.7f }, { 0.4f, 0.9f, 0.2f }, { 0.1f, 0.1f, 1.0f }, { 1.0f, 0.2f, 0.2f } };

        //tri.color = colors[Params.PositionIsoline.w];
        
        triangles.Append(tri);
        InterlockedAdd(indirect[0], 3);
    }
}

[numthreads(8, 8, 8)]
void GenerateMarchingCubes(in uint3 coords : SV_DispatchThreadID)
{   
    if (coords.x >= Params.SizeXYZScale.x - 1)
        return;
    if (coords.y >= Params.SizeXYZScale.y - 1)
        return;
    if (coords.z >= Params.SizeXYZScale.z - 1)
        return;
    
    if (Params.TransitionParams2.w > 0)
    {
        if (Params.TransitionParams2.x > 0 && coords.z < Params.TransitionParams2.x)
        {
            ComputeTransitionNode13(coords, 0);
        }
        else if (Params.TransitionParams.y > 0 && coords.x >= Params.SizeXYZScale.y - 2 - Params.TransitionParams.y)
        {
            ComputeTransitionNode13(coords, 1);
        }
        else if (Params.TransitionParams2.y > 0 && coords.z >= Params.SizeXYZScale.z - 2 - Params.TransitionParams2.y)
        {
            ComputeTransitionNode13(coords, 2);
        }
        else if (Params.TransitionParams.x > 0 && coords.x < Params.TransitionParams.x)
        {
            ComputeTransitionNode13(coords, 3);
        }
        else if (Params.TransitionParams.z > 0 && coords.y < Params.TransitionParams.z)
        {
            ComputeTransitionNode13(coords, 4);
        }
        else if (Params.TransitionParams.w > 0 && coords.y >= Params.SizeXYZScale.y - 1 - Params.TransitionParams.w)
        {
            ComputeTransitionNode13(coords, 5);
        }
        else
        {
            ComputeDefaultNode(coords);
        }
    }
    else
    {
        ComputeDefaultNode(coords);
    }
}
