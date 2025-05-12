#include "pch.h"
#include "PlaneComponent.h"
#include "Game.h"
#include <iostream>
#include "Camera.h"


using namespace DirectX::SimpleMath;



PlaneComponent::PlaneComponent(Game* inGame, Camera* inCamera) : GameComponent(inGame)
{
	camera = inCamera;
}

void PlaneComponent::Initialize()
{
	ID3DBlob* errorCode = nullptr;
	
	auto res = D3DCompileFromFile(
		L"Shaders/Simple.hlsl", 
		nullptr, 
		nullptr, 
		"VSMain", "vs_5_0", 
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0,
		&vertexShaderByteCode, &errorCode);

	if (FAILED(res)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorCode) {
			char* compileErrors = (char*)(errorCode->GetBufferPointer());
	
			std::cout << compileErrors << std::endl; 
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else {
			MessageBox(game->Display->hWnd, L"MiniTri.fx", L"Missing Shader File", MB_OK);
		}
	}

	game->Device->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);

	
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
	
	points = new std::vector<DirectX::SimpleMath::Vector4>();
	
	// Instantiate Vertex buiffer from vertex data
	float dist = 1000.0f;

	for (int i = -10; i < 11; i++) {
		points->emplace_back(Vector4(100.0f * i, 0.0f, dist, 1.0f));		points->emplace_back(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
		points->emplace_back(Vector4(100.0f * i, 0.0f, -dist, 1.0f));		points->emplace_back(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	
		points->emplace_back(Vector4(dist, 0.0f, 100.0f * i, 1.0f));		points->emplace_back(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
		points->emplace_back(Vector4(-dist, 0.0f, 100.0f * i, 1.0f));		points->emplace_back(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	}


	points->emplace_back(Vector4::Zero);			points->emplace_back(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	points->emplace_back(Vector4(1.0f, 0.0f, 0.0f, 1.0f) * 100.0f);	points->emplace_back(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	points->emplace_back(Vector4::Zero);			points->emplace_back(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	points->emplace_back(Vector4(0.0f, 1.0f, 0.0f, 1.0f) * 100.0f);	points->emplace_back(Vector4(0.0f, 1.0f, 0.0f, 1.0f));

	points->emplace_back(Vector4::Zero);			points->emplace_back(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	points->emplace_back(Vector4(0.0f, 0.0f, 1.0f, 1.0f) * 100.0f);	points->emplace_back(Vector4(0.0f, 0.0f, 1.0f, 1.0f));


	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.Usage		= D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags		= 0;
	bufDesc.MiscFlags			= 0;
	bufDesc.StructureByteStride = 32;
	bufDesc.ByteWidth = sizeof(Vector4) * std::size(*points);

	D3D11_SUBRESOURCE_DATA positionsData = {};
	positionsData.pSysMem			= &(*points)[0];
	positionsData.SysMemPitch		= 0;
	positionsData.SysMemSlicePitch	= 0;

	game->Device->CreateBuffer(&bufDesc, &positionsData, &vertices);


	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage		= D3D11_USAGE_DEFAULT;
	constBufDesc.BindFlags	= D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = 0;
	constBufDesc.MiscFlags		= 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(Matrix);

	game->Device->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);
	

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_WIREFRAME;
	rastDesc.AntialiasedLineEnable = true;
	rastDesc.MultisampleEnable = true;

	game->Device->CreateRasterizerState(&rastDesc, &rastState);
}

void PlaneComponent::Update(float deltaTime)
{
	auto proj = camera->ViewMatrix * camera->ProjMatrix;
	
	game->Context->UpdateSubresource(constantBuffer, 0, nullptr, &proj, 0, 0);
}

void PlaneComponent::Draw(float deltaTime)
{
	auto context = game->Context;
	ID3D11RasterizerState* oldState;

	const UINT stride = 32;
	const UINT offset = 0;
	
	context->RSGetState(&oldState);
	context->RSSetState(rastState);

	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	context->IASetVertexBuffers(0, 1, &vertices, &stride, &offset);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->VSSetConstantBuffers(0, 1, &constantBuffer);

	game->DebugAnnotation->BeginEvent(L"Plane Draw Event");
	context->Draw(points->size() / 2, 0);
	game->DebugAnnotation->EndEvent();

	context->RSSetState(oldState);
	SafeRelease(oldState);
}

void PlaneComponent::DestroyResources()
{
	delete points;
	
	pixelShader->Release();
	vertexShader->Release();
	pixelShaderByteCode->Release();
	vertexShaderByteCode->Release();
	layout->Release();
	vertices->Release();

	rastState->Release();

	constantBuffer->Release();
}
