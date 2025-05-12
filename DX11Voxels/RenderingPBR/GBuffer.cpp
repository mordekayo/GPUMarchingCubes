#include "GBuffer.h"

GBuffer::GBuffer(int width, int height)
{
	Resize(width, height);
}

void GBuffer::Resize(int width, int height)
{
	Dispose();

	auto device = Game::Instance->Device;

	auto texDesc = D3D11_TEXTURE2D_DESC{
		static_cast<UINT>(width),
		static_cast<UINT>(height),
		1,
		1,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SAMPLE_DESC{1, 0},
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0,
		0,
	};

	device->CreateTexture2D(&texDesc, nullptr, &DiffuseTex);
	device->CreateTexture2D(&texDesc, nullptr, &EmissiveTex);

	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	device->CreateTexture2D(&texDesc, nullptr, &NormalTex);
	device->CreateTexture2D(&texDesc, nullptr, &MetRougAoIdTex);
	device->CreateTexture2D(&texDesc, nullptr, &WorldPositionTex);

	device->CreateTexture2D(&texDesc, nullptr, &AccumulationBuf);
	device->CreateTexture2D(&texDesc, nullptr, &BloomTex);


	device->CreateShaderResourceView(DiffuseTex, nullptr, &DiffuseSRV);
	device->CreateShaderResourceView(NormalTex, nullptr, &NormalSRV);
	device->CreateShaderResourceView(MetRougAoIdTex, nullptr, &MetRougAoIdSRV);
	device->CreateShaderResourceView(EmissiveTex, nullptr, &EmissiveSRV);
	device->CreateShaderResourceView(WorldPositionTex, nullptr, &WorldPositionSRV);

	device->CreateShaderResourceView(AccumulationBuf, nullptr, &AccumulationSRV);
	device->CreateShaderResourceView(BloomTex, nullptr, &BloomSRV);


	device->CreateRenderTargetView(DiffuseTex, nullptr, &DiffuseRTV);
	device->CreateRenderTargetView(NormalTex, nullptr, &NormalRTV);
	device->CreateRenderTargetView(MetRougAoIdTex, nullptr, &MetRougAoIdRTV);
	device->CreateRenderTargetView(EmissiveTex, nullptr, &EmissiveRTV);
	device->CreateRenderTargetView(WorldPositionTex, nullptr, &WorldPositionRTV);

	device->CreateRenderTargetView(AccumulationBuf, nullptr, &AccumulationRTV);
	device->CreateRenderTargetView(BloomTex, nullptr, &BloomRTV);
}

void GBuffer::Dispose()
{
	SafeRelease(DiffuseRTV);
	SafeRelease(DiffuseSRV);
	SafeRelease(DiffuseTex);
	SafeRelease(NormalRTV);
	SafeRelease(NormalSRV);
	SafeRelease(NormalTex);
	SafeRelease(EmissiveRTV);
	SafeRelease(EmissiveSRV);
	SafeRelease(EmissiveTex);
	SafeRelease(WorldPositionTex);
	SafeRelease(WorldPositionTex);
	SafeRelease(WorldPositionTex);
	SafeRelease(AccumulationBuf);
	SafeRelease(AccumulationBuf);
	SafeRelease(AccumulationBuf);
	SafeRelease(BloomTex);
	SafeRelease(BloomTex);
	SafeRelease(BloomTex);
	SafeRelease(MetRougAoIdSRV);
	SafeRelease(MetRougAoIdRTV);
	SafeRelease(MetRougAoIdTex);
}
