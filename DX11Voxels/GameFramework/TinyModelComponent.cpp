#include "Game.h"
#include "VertexPositionNormalBinormalTangentColorTex.h"
#include "ObjLoader.h"
#include "TextureLoader.h"
#include "TinyModelComponent.h"
#include <iostream>
#include "LightComponent.h"
#include "Camera.h"


using namespace DirectX::SimpleMath;

float TinySpace::GlobalDepthOffset = 0.0f;

#pragma pack(push, 4)
struct ConstDataBuf
{
	Matrix	World;
	Matrix	WorldViewProj;
	Matrix	InvertTransposeWorld;
	Vector4	ViewerPos;
};
#pragma pack(pop)

#pragma pack(push, 4)
struct LightsStruct
{
	Vector4 Direction;
	Vector4 Color;
};
#pragma pack(pop)

ConstDataBuf data	= {};
LightsStruct light	= {};


void TinyModelComponent::DrawToDepth(LightComponent* light)
{
	auto world = Transform * Matrix::CreateTranslation(Position);
	auto proj = world * light->GetViewProjMatrix();

	data.World = world;
	data.WorldViewProj = proj;

	auto context = game->Context;

	context->UpdateSubresource(constantDepthBuffer, 0, nullptr, &data, 0, 0);

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	  
	context->VSSetShader(shadowShader, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

	ID3D11ShaderResourceView* srvs[] = { vSrv, nSrv, tSrv, strSrv };
	context->VSSetShaderResources(0, 4, srvs);

	context->VSSetConstantBuffers(0, 1, &constantDepthBuffer);

	for (int i = 0; i < elemCount; i++) {
		const auto shape = Shapes[i];
		context->DrawIndexed(shape.Count, shape.StartIndex, 0);
	}
}

TinyModelComponent::TinyModelComponent(Game* inGame, Camera* inCamera, LightComponent* inLight, const char* inFileName)
: GameComponent(inGame), MainLight(inLight), camera(inCamera), modelName(inFileName)
{
	
}


void TinyModelComponent::Initialize()
{
	ID3DBlob* errorCode = nullptr;

	auto res = D3DCompileFromFile(
		L"Shaders/TinyShadowShader.hlsl",
		nullptr,
		nullptr,
		"VSMain", "vs_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
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
		L"Shaders/TinyShadowShader.hlsl",
		nullptr,
		nullptr,
		"PSMain", "ps_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&pixelShaderByteCode, &errorCode);

	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);


	res = D3DCompileFromFile(
		L"Shaders/TinyShadowShader.hlsl",
		nullptr,
		nullptr,
		"VSMainShadow", "vs_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0,
		&shadowByteCode, &errorCode);

	game->Device->CreateVertexShader(shadowByteCode->GetBufferPointer(), shadowByteCode->GetBufferSize(), nullptr, &shadowShader);

	
	vBuf = nullptr;
	nBuf = nullptr;
	tBuf = nullptr;
	strBuf = nullptr;
	Materials = nullptr;
	Shapes = nullptr;
	
	game->ObjLoader->LoadTinyModel(modelName, vBuf, nBuf, tBuf, strBuf, Materials, Shapes, elemCount);

	D3D11_BUFFER_DESC descBuf = {};
	vBuf->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = descBuf.ByteWidth / sizeof(Vector3);
	
	game->Device->CreateShaderResourceView(vBuf, &srvDesc, &vSrv);

	nBuf->GetDesc(&descBuf);
	srvDesc.Buffer.NumElements = descBuf.ByteWidth / sizeof(Vector3);
	game->Device->CreateShaderResourceView(nBuf, &srvDesc, &nSrv);


	tBuf->GetDesc(&descBuf);
	srvDesc.Buffer.NumElements = descBuf.ByteWidth / sizeof(Vector2);
	srvDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	game->Device->CreateShaderResourceView(tBuf, &srvDesc, &tSrv);


	strBuf->GetDesc(&descBuf);
	srvDesc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	game->Device->CreateShaderResourceView(strBuf, &srvDesc, &strSrv);

	
	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = 0;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(ConstDataBuf);

	game->Device->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);
	game->Device->CreateBuffer(&constBufDesc, nullptr, &constantDepthBuffer);

	constBufDesc.ByteWidth = sizeof(LightsStruct);
	game->Device->CreateBuffer(&constBufDesc, nullptr, &lightBuffer);
	

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	//rastDesc.AntialiasedLineEnable = true;
	//rastDesc.MultisampleEnable = true;

	game->Device->CreateRasterizerState(&rastDesc, &rastState);



	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; 
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; 

	game->Device->CreateBlendState(&blendDesc, &blendState);
	


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MaxLOD = INT_MAX;
	
	game->Device->CreateSamplerState(&samplerDesc, &sampler);



	D3D11_SAMPLER_DESC depthSamplerDesc = {};
	depthSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	depthSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	depthSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	depthSamplerDesc.Filter			= D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	depthSamplerDesc.ComparisonFunc	= D3D11_COMPARISON_LESS_EQUAL;
	depthSamplerDesc.BorderColor[0] = -1.0f;
	depthSamplerDesc.BorderColor[1] = -1.0f;
	depthSamplerDesc.BorderColor[2] = -1.0f;
	depthSamplerDesc.BorderColor[3] = -1.0f;
	depthSamplerDesc.MaxLOD = INT_MAX;
	
	game->Device->CreateSamplerState(&depthSamplerDesc, &depthSampler);

	int totalCount = 0;
	for (int i = 0; i < elemCount; i++)
	{
		totalCount += Shapes[i].Count;
	}

	int* indexes = new int[totalCount];

	for (int i = 0; i < totalCount; i++)
		indexes[i] = i;

	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * totalCount;

	D3D11_SUBRESOURCE_DATA resData = {};
	resData.pSysMem = indexes;
	resData.SysMemPitch = 0;
	resData.SysMemSlicePitch = 0;

	game->Device->CreateBuffer(&indexBufDesc, &resData, &indexBuffer);

	delete[] indexes;
}

float totalTime = 0;

void TinyModelComponent::Update(float deltaTime)
{
	auto world = Transform * Matrix::CreateTranslation(Position);
	auto proj = world * camera->GetCameraMatrix();

	data.World = world;
	data.WorldViewProj = proj;
	data.InvertTransposeWorld = MainLight->GetViewProjMatrix();
	
	totalTime += deltaTime;
	
	const auto viewerPos = camera->GetPosition();
	data.ViewerPos = Vector4(viewerPos.x, viewerPos.y, viewerPos.z, TinySpace::GlobalDepthOffset);

	light.Color		= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	light.Direction = Vector4(MainLight->Direction.x, MainLight->Direction.y, MainLight->Direction.z, 0.0f);

	game->Context->UpdateSubresource(constantBuffer, 0, nullptr, &data, 0, 0);
	game->Context->UpdateSubresource(lightBuffer, 0, nullptr, &light, 0, 0);
}


void TinyModelComponent::Draw(float deltaTime)
{
	auto context = game->Context;
	ID3D11RasterizerState* oldState;
	
	context->RSGetState(&oldState);
	context->RSSetState(rastState);

	float factors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(blendState, factors, 0xFFFFFFFF);
	
	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	ID3D11ShaderResourceView* srvs[] = {vSrv, nSrv, tSrv, strSrv};
	context->VSSetShaderResources(0, 4, srvs);

	ID3D11Buffer* buffers[] = { constantBuffer, lightBuffer };
	
	context->VSSetConstantBuffers(0, 1, &constantBuffer);
	context->PSSetConstantBuffers(0, 2, buffers);

	ID3D11SamplerState* samplers[] = { sampler, depthSampler };
	context->PSSetSamplers(0, 2, samplers);

	context->PSSetShaderResources(1, 1, &MainLight->DepthShaderView);

	for (int i = 0; i < elemCount; i++) {
		auto shape = Shapes[i];
		auto material = shape.MaterialInd >= 0 ? Materials[shape.MaterialInd] : game->ObjLoader->DefaultMaterial;
		
		context->PSSetShaderResources(0, 1, &material.DiffSRV);

		context->DrawIndexed(shape.Count, shape.StartIndex, 0);
	}
	
	context->RSSetState(oldState);
	oldState->Release();
}

 
void TinyModelComponent::DestroyResources()
{
}
