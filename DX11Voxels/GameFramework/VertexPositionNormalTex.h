#pragma once
#include "Exports.h"


namespace FbxNative {
	class MeshVertex;
}

struct GAMEFRAMEWORK_API VertexPositionNormalTex
{
	DirectX::SimpleMath::Vector4 Position;
	DirectX::SimpleMath::Vector4 Normal;
	DirectX::SimpleMath::Vector4 Tex;

	static const UINT Stride = 48;

	static ID3D11InputLayout* GetLayout(ID3DBlob* signature);

	void Convert(const FbxNative::MeshVertex& v);
};

