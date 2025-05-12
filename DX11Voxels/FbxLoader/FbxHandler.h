#pragma once

#include <fbxsdk.h>
#include "../ZMathLib/SimpleMath.h"
#include "Scene.h"

/*-----------------------------------------------------------------------------
Some helpers :
-----------------------------------------------------------------------------*/

namespace FbxNative {
	class MeshVertex;
	class MeshSubset;


	class FbxHandler
	{
	public:

		FbxHandler();

		
		FbxManager* fbxManager;
		FbxImporter* fbxImporter;
		FbxScene* fbxScene;
		FbxGeometryConverter* fbxGConv;
		FbxTime::EMode			timeMode;

		void IterateChildren(FbxNode* fbxNode, FbxScene* fbxScene, Scene* scene, int parentIndex, int depth);
		void HandleMesh(Scene* scene, Node* node, FbxNode* fbxNode);
		void HandleSkinning(Mesh* nodeMesh, Scene* scene, Node* node, FbxNode* fbxNode, DirectX::SimpleMath::Matrix* meshTransform, std::vector<DirectX::SimpleMath::Int4>& skinIndices, std::vector<DirectX::SimpleMath::Vector4>& skinWeights);
		void HandleCamera(Scene* scene, Node* node, FbxNode* fbxNode);
		void HandleLight(Scene* scene, Node* node, FbxNode* fbxNode);
		void HandleMaterial(MeshSubset* sg, FbxSurfaceMaterial* material);
		void GetNormalForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int ctrlPointId);
		void GetTangentForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int ctrlPointId);
		void GetBinormalForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int ctrlPointId);
		void GetTextureForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int vertexId);
		void GetColorForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int ctrlPointId);

		void GetCustomProperties(Node* node, FbxNode* fbxNode);
	};

}
