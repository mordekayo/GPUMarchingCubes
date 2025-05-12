#include "pch.h"
#include "VertexPositon2DTex2DColor.h"
#include "Game.h"

ID3D11InputLayout* VertexPosition2DTex2DColor::GetLayout(ID3DBlob* signature)
{
	// Layout from VertexShader input signature
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
	};
	
	ID3D11InputLayout* layout;
	Game::Instance->Device->CreateInputLayout(inputElements, 3, signature->GetBufferPointer(), signature->GetBufferSize(), &layout);

	return layout;
}
