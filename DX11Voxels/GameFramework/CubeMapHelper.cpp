#include "CubeMapHelper.h"
#include "../GameFramework/Game.h"
#include "../Misc/DirectXTex/DirectXTex.h"
#include "d3dcompiler.h"
#include <string>
#include <format>
#include <filesystem>
#include <wincodec.h>


void CubeMapHelper::Init(const Game* game)
{
	this->game = game;

	ID3DBlob* errorCode, * vertexShaderByteCode, * geometryShaderByteCode, * pixelShaderByteCode;

	D3DCompileFromFile(L"Shaders/SphereToCube.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &vertexShaderByteCode, &errorCode);
	game->Device->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), nullptr, &vsGenCube);
	SafeRelease(vertexShaderByteCode);
	SafeRelease(errorCode);

	D3DCompileFromFile(L"Shaders/SphereToCube.hlsl", nullptr, nullptr, "GSMain", "gs_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &geometryShaderByteCode, &errorCode);
	game->Device->CreateGeometryShader(geometryShaderByteCode->GetBufferPointer(), geometryShaderByteCode->GetBufferSize(), nullptr, &gsGenCube);
	SafeRelease(geometryShaderByteCode);
	SafeRelease(errorCode);

	D3DCompileFromFile(L"Shaders/SphereToCube.hlsl", nullptr, nullptr, "PSSphericalToCube", "ps_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &pixelShaderByteCode, &errorCode);
	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &psSphericalToCube);
	SafeRelease(pixelShaderByteCode);
	SafeRelease(errorCode);

	D3DCompileFromFile(L"Shaders/SphereToCube.hlsl", nullptr, nullptr, "PSPreFilterCube", "ps_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &pixelShaderByteCode, &errorCode);
	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &psPreFilterCube);
	SafeRelease(pixelShaderByteCode);
	SafeRelease(errorCode);

	D3D11_BUFFER_DESC preFilterConstDesc = {
		.ByteWidth = sizeof(float)*4,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0
	};
	game->Device->CreateBuffer(&preFilterConstDesc , nullptr, &PreFilterConstBuf);

	//if (errorCode) {
	//	char* compileErrors = (char*)(errorCode->GetBufferPointer());
	//
	//	std::cout << compileErrors << std::endl;
	//}


	D3D11_SAMPLER_DESC samplDesc = {};
	{
		samplDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplDesc.BorderColor[0] = 1.0f;
		samplDesc.BorderColor[1] = 0.0f;
		samplDesc.BorderColor[2] = 0.0f;
		samplDesc.BorderColor[3] = 1.0f;
		samplDesc.MaxLOD = static_cast<float>(INT_MAX);
	}
	game->Device->CreateSamplerState(&samplDesc, &sampler);
	

	viewport = {};
	viewport.Width = 2048;
	viewport.Height = 2048;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	ID3D11RasterizerState* rastState;
	game->Device->CreateRasterizerState(&rastDesc, &rastState);


	////////// Convolution part
	D3DCompileFromFile(L"Shaders/ConvolutionCalc.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &pixelShaderByteCode, &errorCode);
	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &psConv);
	SafeRelease(pixelShaderByteCode);
	SafeRelease(errorCode);


	////////// Cube to planar part
	D3DCompileFromFile(L"Shaders/CubeToPlanar.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &vertexShaderByteCode, &errorCode);
	game->Device->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), nullptr, &vsFullscreenPlane);
	SafeRelease(vertexShaderByteCode);
	SafeRelease(errorCode);
	
	D3DCompileFromFile(L"Shaders/CubeToPlanar.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &pixelShaderByteCode, &errorCode);
	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &psCubeToPlanar);
	SafeRelease(pixelShaderByteCode);
	SafeRelease(errorCode);

	D3DCompileFromFile(L"Shaders/CubeToPlanar.hlsl", nullptr, nullptr, "PSIntegrateBRDF", "ps_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &pixelShaderByteCode, &errorCode);
	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &psIntegrateBRDF);
	SafeRelease(pixelShaderByteCode);
	SafeRelease(errorCode);
}

void CubeMapHelper::ReloadShaders()
{
	ID3DBlob* errorCode, * vertexShaderByteCode, * geometryShaderByteCode, * pixelShaderByteCode;

	SafeRelease(psPreFilterCube);
	D3DCompileFromFile(L"Shaders/SphereToCube.hlsl", nullptr, nullptr, "PSPreFilterCube", "ps_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &pixelShaderByteCode, &errorCode);
	game->Device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &psPreFilterCube);
	SafeRelease(pixelShaderByteCode);
	SafeRelease(errorCode);
}


void CubeMapHelper::Convert(ID3D11ShaderResourceView* sphereMap, ZTexViews& outCubeMap)
{
	ID3D11Texture2D* cubeTex = nullptr;
	ID3D11ShaderResourceView* cubeSrv = nullptr;
	ID3D11RenderTargetView* cubeRtv = nullptr;

	D3D11_TEXTURE2D_DESC texDesc = {
		2048,
		2048,
		0,
		6,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_SAMPLE_DESC{1, 0},
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS
	};
	game->Device->CreateTexture2D(&texDesc, nullptr, &cubeTex);
	game->Device->CreateShaderResourceView(cubeTex, nullptr, &cubeSrv);
	game->Device->CreateRenderTargetView(cubeTex, nullptr, &cubeRtv);


	auto context = game->Context;

	context->ClearState();

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->VSSetShader(vsGenCube, nullptr, 0);
	context->GSSetShader(gsGenCube, nullptr, 0);
	context->PSSetShader(psSphericalToCube, nullptr, 0);

	context->PSSetShaderResources(0, 1, &sphereMap);
	context->PSSetSamplers(0, 1, &sampler);

	context->OMSetRenderTargets(1, &cubeRtv, nullptr);
	context->RSSetViewports(1, &viewport);

	context->RSSetState(rastState);


	Game::Instance->DebugAnnotation->BeginEvent(L"Draw sphere to cubemap");

	context->Draw(1, 0);

	Game::Instance->DebugAnnotation->EndEvent();

	context->OMSetRenderTargets(0, nullptr, nullptr);

	context->GenerateMips(cubeSrv);

	outCubeMap.Tex = cubeTex;
	outCubeMap.Srv = cubeSrv;
	outCubeMap.Rtv = cubeRtv;
}


void CubeMapHelper::ConvertHDR(LPCWSTR filePath, ZTexViews& outCubeMap, bool saveResult)
{
	DirectX::ScratchImage image;
	DirectX::LoadFromHDRFile(filePath, nullptr, image);


	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = image.GetMetadata().width;
	desc.Height = image.GetMetadata().height;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem			= image.GetImage(0, 0, 0)->pixels;
	data.SysMemPitch		= image.GetImage(0, 0, 0)->rowPitch;
	data.SysMemSlicePitch	= image.GetImage(0, 0, 0)->slicePitch;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;
	game->Device->CreateTexture2D(&desc, &data, &texture);
	game->Device->CreateShaderResourceView(texture, nullptr, &textureView);

	Convert(textureView, outCubeMap);


	if (saveResult) {
		DirectX::ScratchImage img;
		DirectX::CaptureTexture(game->Device, game->Context, outCubeMap.Tex, img);

		DirectX::SaveToDDSFile(img.GetImages(), img.GetImageCount(), img.GetMetadata(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, std::filesystem::path(filePath).replace_extension("dds").c_str());
		img.Release();
	}

	image.Release();
	textureView->Release();
	texture->Release();
}


void CubeMapHelper::ConvertCubeToPlanar(const ZTexViews& inCubeMap, ID3D11ShaderResourceView*& outSphereMap, bool saveResult)
{
	ID3D11Texture2D*			tex = nullptr;
	ID3D11RenderTargetView*		rtv = nullptr;
	
	D3D11_TEXTURE2D_DESC texDesc = {
		4096,
		2048,
		0,
		1,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SAMPLE_DESC{1, 0},
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		0
	};
	game->Device->CreateTexture2D(&texDesc, nullptr, &tex);
	game->Device->CreateShaderResourceView(tex, nullptr, &outSphereMap);
	game->Device->CreateRenderTargetView(tex, nullptr, &rtv);

	D3D11_VIEWPORT zviewport = {};
	zviewport.Width = 4096;
	zviewport.Height = 2048;
	zviewport.TopLeftX = 0;
	zviewport.TopLeftY = 0;
	zviewport.MinDepth = 0;
	zviewport.MaxDepth = 1.0f;

	auto context = game->Context;

	context->ClearState();

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->VSSetShader(vsFullscreenPlane, nullptr, 0);
	context->PSSetShader(psCubeToPlanar, nullptr, 0);

	context->PSSetShaderResources(0, 1, &inCubeMap.Srv);
	context->PSSetSamplers(0, 1, &sampler);

	context->OMSetRenderTargets(1, &rtv, nullptr);
	context->RSSetViewports(1, &zviewport);

	context->RSSetState(rastState);


	Game::Instance->DebugAnnotation->BeginEvent(L"Draw cube to planar");

	context->Draw(4, 0);

	Game::Instance->DebugAnnotation->EndEvent();

	if (saveResult) {
		DirectX::ScratchImage img;
		DirectX::CaptureTexture(game->Device, game->Context, tex, img);
		DirectX::SaveToWICFile(*img.GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE, GUID_ContainerFormatPng, L"test.png");
		//DirectX::SaveToDDSFile(img.GetImages(), img.GetImageCount(), img.GetMetadata(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, L"test.dds");
	}
}


void CubeMapHelper::CalculateConvolutionMap(const ZTexViews& sourceMap, ZTexViews& outCubeMap)
{
	ID3D11Texture2D* cubeTex = nullptr;
	ID3D11ShaderResourceView* cubeSrv = nullptr;
	ID3D11RenderTargetView* cubeRtv = nullptr;

	D3D11_TEXTURE2D_DESC texDesc = {
		32, // TODO: try different sizes
		32,
		1,
		6,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_SAMPLE_DESC{1, 0},
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE
	};
	game->Device->CreateTexture2D(&texDesc, nullptr, &cubeTex);
	game->Device->CreateShaderResourceView(cubeTex, nullptr, &cubeSrv);
	game->Device->CreateRenderTargetView(cubeTex, nullptr, &cubeRtv);


	auto context = game->Context;

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->VSSetShader(vsGenCube, nullptr, 0);
	context->GSSetShader(gsGenCube, nullptr, 0);
	context->PSSetShader(psConv, nullptr, 0);

	context->PSSetShaderResources(0, 1, &sourceMap.Srv);
	context->PSSetSamplers(0, 1, &sampler);

	context->OMSetRenderTargets(1, &cubeRtv, nullptr);

	D3D11_VIEWPORT smallViewport;
	smallViewport.Width = 32;
	smallViewport.Height = 32;
	smallViewport.TopLeftX = 0;
	smallViewport.TopLeftY = 0;
	smallViewport.MinDepth = 0;
	smallViewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &smallViewport);

	context->RSSetState(rastState);

	
	Game::Instance->DebugAnnotation->BeginEvent(L"Draw convolution map");

	context->Draw(1, 0);

	Game::Instance->DebugAnnotation->EndEvent();

	
	context->OMSetRenderTargets(0, nullptr, nullptr);

	outCubeMap.Tex = cubeTex;
	outCubeMap.Srv = cubeSrv;
	outCubeMap.Rtv = cubeRtv;
}


void CubeMapHelper::PreFilterMipMaps(const ZTexViews& sourceMap, ZTexViews& outCubeMap)
{
	auto context = game->Context;

	ID3D11Texture2D* cubeTex = nullptr;
	ID3D11ShaderResourceView* cubeSrv = nullptr;
	ID3D11RenderTargetView* cubeRtv = nullptr;

	D3D11_TEXTURE2D_DESC texDesc = {
		2048,
		2048,
		0,
		6,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_SAMPLE_DESC{1, 0},
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS
	};
	game->Device->CreateTexture2D(&texDesc, nullptr, &cubeTex);
	game->Device->CreateShaderResourceView(cubeTex, nullptr, &cubeSrv);

	context->GenerateMips(cubeSrv);


	context->ClearState();

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->VSSetShader(vsGenCube, nullptr, 0);
	context->GSSetShader(gsGenCube, nullptr, 0);
	context->PSSetShader(psPreFilterCube, nullptr, 0);

	context->PSSetConstantBuffers(0, 1, &PreFilterConstBuf);

	context->PSSetShaderResources(0, 1, &sourceMap.Srv);
	context->PSSetSamplers(0, 1, &sampler);

	context->RSSetState(rastState);

	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 2048 * std::pow(0.5, mip);
		unsigned int mipHeight = 2048 * std::pow(0.5, mip);

		D3D11_VIEWPORT envViewport = {
			.TopLeftX = 0,
			.TopLeftY = 0,
			.Width = static_cast<float>(mipWidth),
			.Height = static_cast<float>(mipHeight),
			.MinDepth = 0.0f,
			.MaxDepth = 1.0f,
		};
		
		float roughness = (float)mip / (float)(maxMipLevels - 1);

		D3D11_RENDER_TARGET_VIEW_DESC rtDesc = {
			.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY,
			.Texture2DArray = {.MipSlice = mip, .FirstArraySlice = 0, .ArraySize = 6}
		};
		game->Device->CreateRenderTargetView(cubeTex, &rtDesc, &cubeRtv);

		context->OMSetRenderTargets(1, &cubeRtv, nullptr);
		context->RSSetViewports(1, &envViewport);

		DirectX::SimpleMath::Vector4 data = { roughness, 0,0,0 };
		context->UpdateSubresource(PreFilterConstBuf, 0, nullptr, &data.x, 0, 0);

		Game::Instance->DebugAnnotation->BeginEvent(L"PreFilter cubemap at mip: " + mip);
		
		context->Draw(1, 0);

		Game::Instance->DebugAnnotation->EndEvent();
		
		SafeRelease(cubeRtv);
	}
	context->OMSetRenderTargets(0, nullptr, nullptr);


	game->Device->CreateRenderTargetView(cubeTex, nullptr, &cubeRtv);
	outCubeMap.Tex = cubeTex;
	outCubeMap.Srv = cubeSrv;
	outCubeMap.Rtv = cubeRtv;
}


void CubeMapHelper::GenerateIntegratedBRDFMap(ZTexViews& outMap)
{
	D3D11_TEXTURE2D_DESC texDesc = {
		1024,
		1024,
		1,
		1,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_SAMPLE_DESC{1, 0},
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		0
	};
	game->Device->CreateTexture2D(&texDesc, nullptr, &outMap.Tex);
	game->Device->CreateShaderResourceView(outMap.Tex, nullptr, &outMap.Srv);
	game->Device->CreateRenderTargetView(outMap.Tex, nullptr, &outMap.Rtv);

	D3D11_VIEWPORT zviewport = {};
	zviewport.Width = 1024;
	zviewport.Height = 1024;
	zviewport.TopLeftX = 0;
	zviewport.TopLeftY = 0;
	zviewport.MinDepth = 0;
	zviewport.MaxDepth = 1.0f;

	auto context = game->Context;

	context->ClearState();

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->VSSetShader(vsFullscreenPlane, nullptr, 0);
	context->PSSetShader(psIntegrateBRDF, nullptr, 0);

	context->OMSetRenderTargets(1, &outMap.Rtv, nullptr);
	context->RSSetViewports(1, &zviewport);

	context->RSSetState(rastState);


	Game::Instance->DebugAnnotation->BeginEvent(L"Generate IntegratedBRDFMap");

	context->Draw(4, 0);

	Game::Instance->DebugAnnotation->EndEvent();
}


void CubeMapHelper::LoadFromDDS(LPCWSTR filePath, ZTexViews& outCubeMap)
{
	DirectX::ScratchImage img;
	DirectX::LoadFromDDSFile(filePath, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, nullptr, img);

	ID3D11Texture2D* tex = nullptr;
	DirectX::CreateTextureEx(game->Device, 
		img.GetImages(), img.GetImageCount(), img.GetMetadata(), 
		D3D11_USAGE_DEFAULT, 
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 
		0, 0, false, 
		(ID3D11Resource**)&tex);
	
	outCubeMap.Tex = tex;

	game->Device->CreateShaderResourceView(tex, nullptr, &outCubeMap.Srv);
	game->Device->CreateRenderTargetView(tex, nullptr, &outCubeMap.Rtv);

	img.Release();
}

void CubeMapHelper::SaveToDDS(ID3D11Texture2D* tex, LPCWSTR path)
{
	DirectX::ScratchImage img;
	DirectX::CaptureTexture(game->Device, game->Context, tex, img);

	DirectX::SaveToDDSFile(img.GetImages(), img.GetImageCount(), img.GetMetadata(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, std::filesystem::path(path).c_str());

	img.Release();
}

