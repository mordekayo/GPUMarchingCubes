#pragma once
#include "Exports.h"
#include "d3d11.h"
#include "../ZMathLib/SimpleMath.h"
#include "Game.h"
#include <string>
#include "TextureLoader.h"
#include <iostream>

class Game;

namespace FbxNative
{
	class Scene;
}

struct GAMEFRAMEWORK_API ZMaterial
{
	ID3D11Texture2D* DiffuseMap = nullptr;
	ID3D11ShaderResourceView* DiffuseSRV = nullptr;

	ID3D11Texture2D* NormalMap = nullptr;
	ID3D11ShaderResourceView* NormalSRV = nullptr;

	ID3D11Texture2D* RoughnessMap = nullptr;
	ID3D11ShaderResourceView* RoughnessSRV = nullptr;

	ID3D11Texture2D* MetallicMap = nullptr;
	ID3D11ShaderResourceView* MetallicSRV = nullptr;


	float MetallicMult;
	float RoughnessMult;

	void ReloadTextures(const std::wstring& diffuseMap, const std::wstring& normalMap, const std::wstring& roughMap, const std::wstring& metallicMap);
	void ReloadTextures(const std::string& diffuseMap, const std::string& normalMap, const std::string& roughMap, const std::string& metallicMap);
	void ReloadMetallic(const std::wstring& metallicTex);
};


struct GAMEFRAMEWORK_API ZMeshSubset {
	int StartPrimitive;
	int PrimitiveCount;
	int MaterialIndex;
};


class GAMEFRAMEWORK_API StaticMesh
{
public:
	Game* game;

	FbxNative::Scene* scene;

	std::string MeshName;
	bool IsSkinned = false;

	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
	UINT Stride;

	ID3D11Buffer* AnimFrameBuf = nullptr;
	ID3D11ShaderResourceView* AnimFrame;


	int	VertexCount;
	int	IndexCount;

	std::vector<ZMeshSubset>	Subsets;
	std::vector<ZMaterial>		Materials;

	DirectX::SimpleMath::Matrix Transform		;
	DirectX::SimpleMath::Matrix LocalTransform	;

	StaticMesh(Game* inGame, const std::string& meshName);
	StaticMesh(Game* inGame, const std::string& meshName, ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int vertCount, int indCount, int stride);

	void ReloadMesh(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int vertCount, int indCount, int stride);
	void Reload(std::string meshName);

	void InitSkeletalData();

	void SetAnimTime(float timeSec);

	~StaticMesh();
};

