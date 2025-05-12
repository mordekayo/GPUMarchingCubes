#include "ReflectionSphereComp.h"

#include "../FbxLoader/Mesh.h"
#include "../GameFramework/Camera.h"
#include "../GameFramework/VertexPositionNormalTex.h"


ReflectionSphereComp::ReflectionSphereComp(Game* game, Camera* cam): GameComponent(game)
{
	this->cam = cam;
}


void ReflectionSphereComp::DestroyResources()
{
}


void ReflectionSphereComp::Draw(float deltaTime)
{
	auto context = game->Context;

	ID3D11RasterizerState* oldState = nullptr;
	game->Context->RSGetState(&oldState);

	game->Context->RSSetState(rastState);

	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexPositionNormalTex::Stride, &offset);

	context->VSSetShader(vs, nullptr, 0);
	context->PSSetShader(ps, nullptr, 0);

	context->VSSetConstantBuffers(0, 1, &constBuf);
	context->PSSetSamplers(0, 1, &sampler);
	context->PSSetShaderResources(0, 1, &CubeMapSRV);
	context->PSSetConstantBuffers(0, 1, &constBuf);

	game->DebugAnnotation->BeginEvent(L"Reflection Sphere Draw Event");
	context->Draw(VertsCount, 0);
	game->DebugAnnotation->EndEvent();

	game->Context->RSSetState(oldState);
	SafeRelease(oldState);
}


void ReflectionSphereComp::Initialize()
{
	auto scene = game->FbxLoader->LoadScene("content/refSphere.FBX");
	auto mesh = scene->Meshes[0];

	VertsCount = mesh->GetVertexCount();

	VertexPositionNormalTex* verts = new VertexPositionNormalTex[VertsCount];

	for (int ind = 0; ind < VertsCount; ind++)
	{
		auto pos = mesh->Vertices[ind].Position;

		verts[ind] = VertexPositionNormalTex{
			Vector4(pos, 1.0f),
			Vector4(mesh->Vertices[ind].Normal, 0.0f),
			Vector4()
		};
	}


	D3D11_SUBRESOURCE_DATA vertsData{
		verts,
		0,
		0
	};

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.ByteWidth = VertsCount * VertexPositionNormalTex::Stride;
	game->Device->CreateBuffer(&bufDesc, &vertsData, &VertexBuffer);


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

	D3DCompileFromFile(L"Shaders/ReflectionSphere.hlsl", nullptr, nullptr, "VSMain", "vs_5_0",
	                   D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
	                   0, &vertexShaderByteCode, &errorCode);
	game->Device->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(),
	                                 nullptr, &vs);
	SafeRelease(errorCode);

	D3DCompileFromFile(L"Shaders/ReflectionSphere.hlsl", nullptr, nullptr, "PSMain", "ps_5_0",
	                   D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
	                   0, &pixelShaderByteCode, &errorCode);
	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(),
	                                nullptr, &ps);
	SafeRelease(errorCode);

	int stride;
	layout = VertexPositionNormalTex::GetLayout(vertexShaderByteCode);

	SafeRelease(vertexShaderByteCode);
	SafeRelease(pixelShaderByteCode);


	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

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
}


void ReflectionSphereComp::Update(float deltaTime)
{
	cData.World = DirectX::SimpleMath::Matrix::CreateScale(10) * DirectX::SimpleMath::Matrix::CreateTranslation(
		DirectX::SimpleMath::Vector3(0, 0, 0));
	cData.View = cam->ViewMatrix;
	cData.Proj = cam->ProjMatrix;
	cData.ViewerPosition = cam->GetPosition();

	game->Context->UpdateSubresource(constBuf, 0, nullptr, &cData, 0, 0);
}
