#pragma once
#include "Exports.h"


namespace FbxNative {
	class MeshVertex;
}

struct GAMEFRAMEWORK_API VertexPositionNormalBinormalTangentColorTex
{
	VertexPositionNormalBinormalTangentColorTex() = default;
	VertexPositionNormalBinormalTangentColorTex(
		DirectX::SimpleMath::Vector4 position,
		DirectX::SimpleMath::Vector4 normal,
		DirectX::SimpleMath::Vector4 binormal,
		DirectX::SimpleMath::Vector4 tangent,
		DirectX::SimpleMath::Vector4 color,
		DirectX::SimpleMath::Vector4 tex
		) : Position(position), Normal(normal), Binormal(binormal), Tangent(tangent), Color(color), Tex(tex)
	{}
	
	DirectX::SimpleMath::Vector4 Position;
	DirectX::SimpleMath::Vector4 Normal;
	DirectX::SimpleMath::Vector4 Binormal;
	DirectX::SimpleMath::Vector4 Tangent;
	DirectX::SimpleMath::Vector4 Color;
	DirectX::SimpleMath::Vector4 Tex;

	static const int Stride = 96;

	static ID3D11InputLayout* GetLayout(ID3DBlob* signature);

	VertexPositionNormalBinormalTangentColorTex& operator= (const VertexPositionNormalBinormalTangentColorTex& other);

	void Convert(const FbxNative::MeshVertex& v);
};

