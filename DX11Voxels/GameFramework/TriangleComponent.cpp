#include "pch.h"
#include "Game.h"
#include "TriangleComponent.h"
#include "Camera.h"

using namespace  DirectX::SimpleMath;

TriangleComponent::TriangleComponent(Game* inGame, Camera* inCamera) : GameComponent(inGame)
{
	camera = inCamera;

	Position = Vector3::Zero;
}

void TriangleComponent::Initialize()
{
	ID3DBlob* errorCode;

	auto res = D3DCompileFromFile(
		L"Shaders/Simple.hlsl",
		nullptr,
		nullptr,
		"VSMain", "vs_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0,
		&vertexShaderByteCode, &errorCode);
	game->Device->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);

	auto t = std::rand();

	RAND_MAX;

	res = D3DCompileFromFile(
		L"Shaders/Simple.hlsl",
		nullptr,
		nullptr,
		"PSMain", "ps_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0,
		&pixelShaderByteCode, &errorCode);
	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);

	// Layout from VertexShader input signature
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};
	game->Device->CreateInputLayout(inputElements, 2, vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &layout);

	points = new Vector4[6]{
		Vector4(0.0f,		50.5f,	0.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
		Vector4(50.5f,	-50.5f,	0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f),
		Vector4(-50.5f,	-50.5f,	0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)
	};

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 32;
	bufDesc.ByteWidth = sizeof(Vector4) * 6;

	D3D11_SUBRESOURCE_DATA positionsData = {};
	positionsData.pSysMem		= points;
	positionsData.SysMemPitch	= 0;
	positionsData.SysMemSlicePitch = 0;

	game->Device->CreateBuffer(&bufDesc, &positionsData, &vertices);

	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(Matrix);

	game->Device->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	game->Device->CreateRasterizerState(&rastDesc, &rastState);

	game->Context->QueryInterface(IID_ID3DUserDefinedAnnotation, (void**)& annotation);
}

void TriangleComponent::Update(float deltaTime)
{
	auto wvp = Matrix::CreateTranslation(Position) * /*Matrix::CreateRotationY(game->TotalTime) **/ camera->ViewMatrix * camera->ProjMatrix;

	//game->Context->UpdateSubresource(constantBuffer, 0, nullptr, &wvp, 0, 0);

	D3D11_MAPPED_SUBRESOURCE res = {};
	game->Context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	auto dataP = reinterpret_cast<float*>(res.pData);
	memcpy(dataP, &wvp, sizeof(Matrix));

	game->Context->Unmap(constantBuffer, 0);
}

void TriangleComponent::Draw(float deltaTime)
{
	auto context = game->Context;
	ID3D11RasterizerState* oldState;

	const UINT stride = 32;
	const UINT offset = 0;

	context->RSGetState(&oldState);
	context->RSSetState(rastState);

	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 1, &vertices, &stride, &offset);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->VSSetConstantBuffers(0, 1, &constantBuffer);

	annotation->BeginEvent(L"Triangle Draw Event");
	context->Draw(3, 0);
	annotation->EndEvent();

	context->RSSetState(oldState);
	oldState->Release();
}

void TriangleComponent::DestroyResources()
{
	delete[] points;

	pixelShader->Release();
	vertexShader->Release();
	pixelShaderByteCode->Release();
	vertexShaderByteCode->Release();
	layout->Release();
	vertices->Release();

	rastState->Release();

	constantBuffer->Release();
	annotation->Release();
}
