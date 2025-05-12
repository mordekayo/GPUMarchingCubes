#pragma once
#include "Exports.h"


namespace FbxNative {
	class MeshVertex;
}

struct GAMEFRAMEWORK_API VertexSkin
{
	VertexSkin() = default;
	VertexSkin(
		DirectX::SimpleMath::Vector4	position,
		DirectX::SimpleMath::Vector4	normal,
		DirectX::SimpleMath::Vector4	binormal,
		DirectX::SimpleMath::Vector4	tangent,
		DirectX::SimpleMath::Vector4	color,
		DirectX::SimpleMath::Vector4	tex,
		DirectX::SimpleMath::Int4		inds,
		DirectX::SimpleMath::Vector4	weights
		) : Position(position), Normal(normal), Binormal(binormal), Tangent(tangent), Color(color), Tex(tex), Inds(inds), Weights(weights)
	{}
	
	DirectX::SimpleMath::Vector4	Position;
	DirectX::SimpleMath::Vector4	Normal;
	DirectX::SimpleMath::Vector4	Binormal;
	DirectX::SimpleMath::Vector4	Tangent;
	DirectX::SimpleMath::Vector4	Color;
	DirectX::SimpleMath::Vector4	Tex;
	DirectX::SimpleMath::Int4		Inds;
	DirectX::SimpleMath::Vector4	Weights;

	static const int Stride = 128;

	static ID3D11InputLayout* GetLayout(ID3DBlob* signature);

	VertexSkin& operator= (const VertexSkin& other);

	void Convert(const FbxNative::MeshVertex& v);
};

