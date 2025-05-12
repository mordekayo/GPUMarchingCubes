#include "pch.h"
#include "VertexSkin.h"
#include "Game.h"
#include "../FbxLoader/MeshVertex.h"


ID3D11InputLayout* VertexSkin::GetLayout(ID3DBlob* signature)
{
	// Layout from VertexShader input signature
	static D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"BINORMAL",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"TANGENT",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"BLENDINDICES",
			0,
			DXGI_FORMAT_R32G32B32A32_SINT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"BLENDWEIGHT",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
	};
	ID3D11InputLayout* layout;
	Game::Instance->Device->CreateInputLayout(inputElements, 8, signature->GetBufferPointer(), signature->GetBufferSize(), &layout);

	return layout;
}

VertexSkin& VertexSkin::operator=(
	const VertexSkin& other)
{
	Position	= other.Position;
	Normal		= other.Normal;
	Binormal	= other.Binormal;
	Tangent		= other.Tangent;
	Color		= other.Color;
	Tex			= other.Tex;
	Inds		= other.Inds;
	Weights		= other.Weights;

	return *this;
}

void VertexSkin::Convert(const FbxNative::MeshVertex& v)
{
	Position	= Vector4(v.Position.x, v.Position.y, v.Position.z, 1.0f);
	Normal		= Vector4(v.Normal.x, v.Normal.y, v.Normal.z, 0.0f);
	Binormal	= Vector4(v.Binormal.x, v.Binormal.y, v.Binormal.z, 0.0f);
	Tangent		= Vector4(v.Tangent.x, v.Tangent.y, v.Tangent.z, 0.0f);
	Color	= Vector4(v.Color0);
	Tex		= Vector4(v.TexCoord0.x, v.TexCoord0.y, v.TexCoord1.x, v.TexCoord1.y);
	Inds	= v.SkinIndices;
	Weights = v.SkinWeights;
}
