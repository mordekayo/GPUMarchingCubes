#pragma once
#include "Exports.h"


namespace FbxNative {
	class MeshVertex;
}

struct GAMEFRAMEWORK_API VertexPositionColor
{
	VertexPositionColor() = default;
	VertexPositionColor(DirectX::SimpleMath::Vector4 position, DirectX::SimpleMath::Vector4 color) : Position(position), Color(color)
	{}
	
	DirectX::SimpleMath::Vector4 Position;
	DirectX::SimpleMath::Vector4 Color;

	static const UINT Stride = 32;

	static ID3D11InputLayout* GetLayout(ID3DBlob* signature);

	void Convert(const FbxNative::MeshVertex& v);
};
