#include "SkyBoxComp.h"

#include "../FbxLoader/Mesh.h"
#include "../GameFramework/Camera.h"
#include "../GameFramework/TextureLoader.h"
#include "../GameFramework/VertexPositionNormalTex.h"
#include "../GameFramework/InputDevice.h"
#include "../GameFramework/DebugRenderSystem.h"
#include <VertexPositionColor.h>
#include <iostream>



SkyBoxGameComponent::SkyBoxGameComponent(Game* game, Camera* cam): GameComponent(game), SRV(nullptr), cubeMap(nullptr)
{
	this->cam = cam;
}

void SkyBoxGameComponent::DestroyResources()
{
}

void SkyBoxGameComponent::Draw(float deltaTime)
{
	auto context = game->Context;

	ID3D11RasterizerState* oldState = nullptr;
	game->Context->RSGetState(&oldState);

	game->Context->RSSetState(rastState);


	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &VertexPositionColor::Stride, &offset);
	context->VSSetShader(vs, nullptr, 0);
	context->PSSetShader(ps, nullptr, 0);

	context->VSSetConstantBuffers(0, 1, &constBuf);
	context->PSSetSamplers(0, 1, &sampler);
	context->PSSetShaderResources(0, 1, &SRV);

	game->DebugAnnotation->BeginEvent(L"Skybox Draw Event");
	context->Draw(vertsCount, 0);
	game->DebugAnnotation->EndEvent();

	game->Context->RSSetState(oldState);
	SafeRelease(oldState);
}


void SkyBoxGameComponent::Initialize()
{
	if (isInitialized) return;

	//LPCWSTR skyFiles[] = {
	//	L"./Content/Left/lf.png",
	//	L"./Content/Left/rt.png",
	//	L"./Content/Left/up.png",
	//	L"./Content/Left/dn.png",
	//	L"./Content/Left/bk.png",
	//	L"./Content/Left/ft.png",
	//};
	//
	//std::vector<ID3D11Texture2D*> skyList;
	//
	//for (auto& skyFile : skyFiles)
	//{
	//	ID3D11Texture2D* tex = nullptr;
	//	ID3D11ShaderResourceView* res = nullptr;
	//	game->TextureLoader->LoadTextureFromFile(skyFile, tex, res, false, false);
	//	skyList.push_back(tex);
	//
	//	SafeRelease(res);
	//}
	//
	//D3D11_TEXTURE2D_DESC firstDesc;
	//skyList.front()->GetDesc(&firstDesc);
	//D3D11_TEXTURE2D_DESC texDesc = {
	//	firstDesc.Width,
	//	firstDesc.Height,
	//	1,
	//	6,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	DXGI_SAMPLE_DESC{1, 0},
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_SHADER_RESOURCE,
	//	0,
	//	D3D11_RESOURCE_MISC_TEXTURECUBE
	//};
	//game->Device->CreateTexture2D(&texDesc, nullptr, &cubeMap);
	//game->Device->CreateShaderResourceView(cubeMap, nullptr, &SRV);
	//
	//int i = 0;
	//for (auto& skytex : skyList)
	//{
	//	game->Context->CopySubresourceRegion(cubeMap, i++, 0, 0, 0, skytex, 0, nullptr);
	//	SafeRelease(skytex);
	//}



	auto scene = game->FbxLoader->LoadScene("content/skySphere2.FBX");
	auto mesh = scene->Meshes[0];

	vertsCount = mesh->GetVertexCount();

	VertexPositionColor* verts = new VertexPositionColor[vertsCount];

	for (int ind = 0; ind < vertsCount; ind++)
	{
		auto pos = mesh->Vertices[ind].Position;

		auto normal = scene->Meshes[0]->Vertices[ind].Normal;

		verts[ind] = VertexPositionColor{
			Vector4(pos, 1.0f),
			Vector4(normal, 1.0f)
		};
	}

	D3D11_SUBRESOURCE_DATA vertsData { verts, 0, 0 };

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.ByteWidth = vertsCount * VertexPositionColor::Stride;
	game->Device->CreateBuffer(&bufDesc, &vertsData, &vertexBuffer);

	D3D11_BUFFER_DESC constDesc = {};
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.CPUAccessFlags = 0;
	constDesc.MiscFlags = 0;
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	constDesc.ByteWidth = sizeof(ConstData);
	game->Device->CreateBuffer(&constDesc, nullptr, &constBuf);

	delete scene;
	delete[] verts;

	ID3DBlob *errorCode, *vertexShaderByteCode, *pixelShaderByteCode;

	D3DCompileFromFile(L"Shaders/skybox.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &vertexShaderByteCode, &errorCode);
	game->Device->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), nullptr, &vs);
	SafeRelease(errorCode);

	D3DCompileFromFile(L"Shaders/skybox.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &pixelShaderByteCode, &errorCode);
	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &ps);
	SafeRelease(errorCode);

	layout = VertexPositionColor::GetLayout(vertexShaderByteCode);

	SafeRelease(vertexShaderByteCode);
	SafeRelease(pixelShaderByteCode);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthClipEnable = false;

	game->Device->CreateRasterizerState(&rastDesc, &rastState);


	D3D11_SAMPLER_DESC samplDesc = {};
	{
		samplDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplDesc.BorderColor[0] = 1.0f;
		samplDesc.BorderColor[1] = 0.0f;
		samplDesc.BorderColor[2] = 0.0f;
		samplDesc.BorderColor[3] = 1.0f;
		samplDesc.MaxLOD = static_cast<float>(INT_MAX);
	}
	game->Device->CreateSamplerState(&samplDesc, &sampler);


	isInitialized = true;
}


void SkyBoxGameComponent::Update(float deltaTime)
{
	cData.World = DirectX::SimpleMath::Matrix::CreateScale(1);// * DirectX::SimpleMath::Matrix::CreateTranslation(cam->GetPosition());
	cData.View = cam->ViewMatrix;
	cData.Proj = cam->ProjMatrix;

	cData.View.Translation(Vector3(0, 0, 0));

	game->Context->UpdateSubresource(constBuf, 0, nullptr, &cData, 0, 0);
}
