#pragma once
#include "Exports.h"


namespace FbxNative {
	class MeshVertex;
}

struct GAMEFRAMEWORK_API VertexPositionTex
{
	DirectX::SimpleMath::Vector4 Position;
	DirectX::SimpleMath::Vector4 Tex;

	static const int Stride = 32;

	static ID3D11InputLayout* GetLayout(ID3DBlob* signature);

	void Convert(const FbxNative::MeshVertex& v);
};

