#pragma once

#pragma warning(disable : 4275)
#pragma warning(disable : 4251)

#include "MeshVertex.h"
#include "MeshTriangle.h"
#include "MeshSubset.h"
#include <vector>

#ifdef FBXLOADER_EXPORTS  
#define FBXLOADER_API __declspec(dllexport)   
#else  
#define FBXLOADER_API __declspec(dllimport)   
#endif


namespace FbxNative {

	class FBXLOADER_API Mesh
	{
	public:
		bool isSkinned = false;

		std::vector<MeshVertex>		Vertices;
		std::vector<MeshTriangle>	Triangles;
		std::vector<MeshSubset>		Subsets;


		int	GetTriangleCount() const { return (int)Triangles.size(); }
		int	GetVertexCount() const { return (int)Vertices.size(); }
		int	GetIndexCount() const { return (int)GetTriangleCount() * 3; }


		/// <summary>
		/// Mesh constructor
		/// </summary>
		Mesh()
		{
			Vertices	= std::vector<MeshVertex>();
			Triangles	= std::vector<MeshTriangle>();
			Subsets		= std::vector<MeshSubset>();
		}

	};

}
