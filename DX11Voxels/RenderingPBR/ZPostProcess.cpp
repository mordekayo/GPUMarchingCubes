#include "ZPostProcess.h"
#include <d3dcompiler.h>
#include <iostream>
#include <Game.h>
#include <Camera.h>
#include "GBuffer.h"
#include "Lights.h"



void ZPostProcess::Init()
{
	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;

	auto res = D3DCompileFromFile(L"./Shaders/PostProcVSScreenQuad.hlsl",
		nullptr,
		nullptr/*Game::Instance->IncludeShaderHandler*/,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexBC,
		&errorVertexCode
	);

	if (FAILED(res)) {
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else {
			std::cout << "Missing Shader File: " << "./Shaders/PostProcVSScreenQuad.hlsl" << std::endl;;
		}

		return;
	}

	Game::Instance->Device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &vsScreenQuad);


	Reload();
}


void ZPostProcess::Update(float dTime)
{
}


void ZPostProcess::Draw(const GBuffer& gBuffer, float dTime)
{
	auto game = Game::Instance;

	if (rtv == nullptr) {
		D3D11_TEXTURE2D_DESC texDesc;
		gBuffer.AccumulationBuf->GetDesc(&texDesc);

		game->Device->CreateTexture2D(&texDesc, nullptr, &tex);
		game->Device->CreateRenderTargetView(tex, nullptr, &rtv);
	}


	game->DebugAnnotation->BeginEvent(L"Post P Pass");

	auto context = game->Context;

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->ClearRenderTargetView(rtv, clearColor);

	context->OMSetRenderTargets(1, &rtv, nullptr);
	context->RSSetViewports(1, &game->ScreenViewport);


	context->RSSetState(game->RastStateCullBack);
	context->OMSetBlendState(game->BlendStateOpaque, nullptr, 0xffffffff);
	context->OMSetDepthStencilState(game->DepthStateNoDepth, 0);

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	context->VSSetShader(vsScreenQuad, nullptr, 0);
	context->PSSetShader(ps, nullptr, 0);

	//constData.World = mesh.Transform;
	//constData.WorldViewProj = constData.World * constData.View * constData.Proj;

	//context->UpdateSubresource(constBuf, 0, nullptr, &constData, 0, 0);

	context->PSSetConstantBuffers(0, 1, &constBuf);


	ID3D11ShaderResourceView* resources[] = {gBuffer.DiffuseSRV, gBuffer.NormalSRV, gBuffer.MetRougAoIdSRV, gBuffer.EmissiveSRV, gBuffer.WorldPositionSRV, gBuffer.AccumulationSRV};

	context->PSSetShaderResources(0, 6, resources);


	//context.InputAssembler.SetVertexBuffers(0, null, null, null);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

	context->Draw(4, 0);

	game->DebugAnnotation->EndEvent();


	if (CopyToAcc) {
		context->CopyResource(gBuffer.AccumulationBuf, tex);
	}
}


void ZPostProcess::Reload()
{
	SafeRelease(ps);

	ID3DBlob* pixelBC = nullptr;
	ID3DBlob* errorPixelCode = nullptr;

	auto res = D3DCompileFromFile(pixelShaderPath.c_str(),
		nullptr,
		nullptr/*Game::Instance->IncludeShaderHandler*/,
		psEntryPoint.c_str(),
		"ps_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelBC,
		&errorPixelCode
	);

	if (FAILED(res)) {
		if (errorPixelCode) {
			char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else {
			std::cout << "Missing Shader File: " << "./Shaders/PostProcVSScreenQuad.hlsl" << std::endl;;
		}

		return;
	}

	Game::Instance->Device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &ps);
}


void ZAtmospherePostProc::Init()
{
	pixelShaderPath = "./Shaders/PostProcPS.hlsl";
	psEntryPoint = "PSMain";
	
	ZPostProcess::Init();

	D3D11_BUFFER_DESC constDesc = {};
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.CPUAccessFlags = 0;
	constDesc.MiscFlags = 0;
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	constDesc.ByteWidth = sizeof(ZAtmospherePostProc::AtmosConstData);
	Game::Instance->Device->CreateBuffer(&constDesc, nullptr, &constBuf);


	// Default params
	data.beta_R = DirectX::SimpleMath::Vector3(0.0058f, 0.0135f, 0.0331f);
	data.H_R = 7.994f;

	data.beta_M = 0.021f;
	data.H_M = 1.2f;
	data.g = 0.888f;

	data.lightSamples = 20;
	data.viewSamples = 20;

	data.R_e = 30;
	data.R_a = 40;
}


void ZAtmospherePostProc::Update(float dTime)
{
	auto game = Game::Instance;

	auto vpMat = game->GameCamera->GetCameraMatrix();
	vpMat.Invert(data.invVP);

	data.viewPos = game->GameCamera->GetPosition();

	data.sunPos = dirLight.PosDir;
	data.I_sun	= dirLight.Intensity;

	//std::cout << data.viewPos.x << " " << data.viewPos.y << " " << data.viewPos.z << "\n";

	Game::Instance->Context->UpdateSubresource(constBuf, 0, nullptr, &data, 0, 0);
}

void ZAtmosCodingAdventure::Init()
{
	pixelShaderPath = "./Shaders/AtmosCodingAdventurePS.hlsl";
	psEntryPoint = "PSMain";

	ZPostProcess::Init();

	D3D11_BUFFER_DESC constDesc = {};
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.CPUAccessFlags = 0;
	constDesc.MiscFlags = 0;
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	constDesc.ByteWidth = sizeof(ZAtmosCodingAdventure::CodingAdventureData);
	Game::Instance->Device->CreateBuffer(&constDesc, nullptr, &constBuf);

	// Default params
	data.PlanetCenter = { 0,0,0 };
	data.PlanetRadius = 30;
	data.AtmosphereRadius = 50;

	data.DensityFalloff = 0.0f;

	data.numScatteringPoints = 10;
	data.numOpticalDepthPoints = 10;
}

void ZAtmosCodingAdventure::Update(float dTime)
{
	auto game = Game::Instance;

	auto vpMat = game->GameCamera->GetCameraMatrix();
	vpMat.Invert(data.invVP);

	data.viewPos = game->GameCamera->GetPosition();

	data.sunDir = dirLight.PosDir;
	data.I_sun = dirLight.Intensity;

	//std::cout << data.viewPos.x << " " << data.viewPos.y << " " << data.viewPos.z << "\n";
	
	Game::Instance->Context->UpdateSubresource(constBuf, 0, nullptr, &data, 0, 0);
}
