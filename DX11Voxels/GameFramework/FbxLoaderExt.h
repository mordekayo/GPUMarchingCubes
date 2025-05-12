#pragma once
#include <filesystem>
#include "Exports.h"
#include "../FbxLoader/Mesh.h"
#include "../FbxLoader/MaterialRef.h"
#include "../FbxLoader/Node.h"
#include "../FbxLoader/Scene.h"
#include "VertexPositionNormalBinormalTangentColorTex.h"
#include "StaticMesh.h"




template<typename T>
void ConvertStaticMeshToVertices(const FbxNative::Mesh* mesh, std::vector<T>& verts, std::vector<int>& inds)
{
	auto triCount = mesh->GetTriangleCount();
	auto vetCount = mesh->GetVertexCount();

	verts.clear();
	inds.clear();
	
	verts.resize(vetCount);
	inds.resize(triCount * 3);
	
	for (int i = 0; i < triCount; i++) {
		auto& tri = mesh->Triangles[i];
	
		inds[i * 3 + 0] = tri.Index0;
		inds[i * 3 + 1] = tri.Index2;
		inds[i * 3 + 2] = tri.Index1;
	}
	
	for (int i = 0; i < vetCount; i++) {
		auto& ver = mesh->Vertices[i];
	
		verts[i].Convert(ver);
	}
}


void GAMEFRAMEWORK_API LoadSubmeshesWithMaterials(const std::string& fileName, const FbxNative::Mesh* fbxMesh, const FbxNative::Scene* scene, StaticMesh* mesh);


template<typename T>
StaticMesh* LoadFbxStaticMesh(std::string fileName)
{
	auto scene = Game::Instance->FbxLoader->LoadScene(fileName);

	if (scene->Meshes.size() > 1) std::cout << "Warning: There is more than one mesh in " << fileName << " but loaded only first\n";
	if (scene->Meshes.size() < 1) {
		std::string msg = "No meshes found in : " + fileName;
		throw std::exception(msg.c_str());
	}

	std::vector<T> verts;
	std::vector<int> inds;

	ConvertStaticMeshToVertices<T>(scene->Meshes[0], verts, inds);
	
	auto verticesCount = verts.size();
	auto indecesCount = inds.size();

	ID3D11Buffer* vertexBuffer;

	D3D11_BUFFER_DESC dataBufDesc = {};
	dataBufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	dataBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dataBufDesc.CPUAccessFlags = 0;
	dataBufDesc.MiscFlags = 0;
	dataBufDesc.StructureByteStride = 0;
	dataBufDesc.ByteWidth = verticesCount * T::Stride;

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = &verts[0];
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;

	Game::Instance->Device->CreateBuffer(&dataBufDesc, &initialData, &vertexBuffer);

	ID3D11Buffer* indexBuffer;
	dataBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dataBufDesc.ByteWidth = 4 * indecesCount;
	initialData.pSysMem = &inds[0];

	Game::Instance->Device->CreateBuffer(&dataBufDesc, &initialData, &indexBuffer);
	
	auto mesh = new StaticMesh(Game::Instance, fileName , vertexBuffer, indexBuffer, verticesCount, indecesCount, T::Stride);
	
	LoadSubmeshesWithMaterials(fileName, scene->Meshes[0], scene, mesh);


	delete scene;

	return mesh;
}


std::vector<StaticMesh*> GAMEFRAMEWORK_API LoadAllStaticMeshes(const std::string& fileName);

