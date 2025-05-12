#pragma once
#include "Exports.h"

#include "Game.h"


struct VertexPositionNormalBinormalTangentColorTex;
class Game;


struct GAMEFRAMEWORK_API TinyMaterial
{
	ID3D11Texture2D* DiffuseTexture;
	ID3D11ShaderResourceView* DiffSRV;
	const char* TexName;
};


struct GAMEFRAMEWORK_API TinyShape
{
	int StartIndex;
	int Count;

	int MaterialInd;
};


class GAMEFRAMEWORK_API ObjLoader
{
	Game* game;

public:
	TinyMaterial DefaultMaterial;

public:
	ObjLoader(Game* inGame);
	
	void LoadObjModel(LPCWSTR fileName, ID3D11Buffer*& vBuffer, int& vCount);
	void LoadVertices(LPCWSTR fileName, VertexPositionNormalBinormalTangentColorTex*& vertices, int& vCount) const;

	void LoadTinyModel(const char* fileName, ID3D11Buffer*& vBuf, ID3D11Buffer*& nBuf, ID3D11Buffer*& tBuf, ID3D11Buffer*& strBuf, TinyMaterial*& outMaterials, TinyShape*& outShapes, int& shapesCount);
	void GatherLoadResults(const char* fileName, float*& positions, int& positionsCount, int*& triangles, int& trianglesCount);
};

