#pragma once
#include "../ZMathLib/SimpleMath.h"

#ifdef FBXLOADER_EXPORTS  
#define FBXLOADER_API __declspec(dllexport)   
#else  
#define FBXLOADER_API __declspec(dllimport)   
#endif


namespace FbxNative {

	class Mesh;


	class FBXLOADER_API MeshTriangle
	{
	public:

		int		Index0;
		int		Index1;
		int		Index2;
		int		MaterialIndex;

		MeshTriangle() = default;
		
		MeshTriangle(int i0, int i1, int i2, int mtrlId) {
			Index0 = i0;
			Index1 = i1;
			Index2 = i2;
			MaterialIndex = mtrlId;
		}

		bool IsDegenerate();

		DirectX::SimpleMath::Vector3 ComputeNormal(Mesh* mesh);
		DirectX::SimpleMath::Vector3 Centroid(Mesh* mesh);
	};

}
