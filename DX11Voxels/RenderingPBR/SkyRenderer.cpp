#include "SkyRenderer.h"

#include "Camera.h"
#include "TextureLoader.h"
#include "../Misc/DirectXTex/DirectXTex.h"

using namespace DirectX::SimpleMath;

struct vs_sky_cbuffer
{
	Matrix g_matViewProj;
};

struct ps_sky_cbuffer
{
	Vector3 g_sunDirection;
	float g_sunIntensity;
	float g_skyRotation;
	float g_rayleighScattering;
	float g_mieScattering;
	float g_useDynamicSkyTexture;
};

#ifndef PI 
#define PI 3.141593f
#endif



SkyRenderer::SkyRenderer(Game* inGame) : game(inGame)
{
}

void SkyRenderer::Update(float deltaTime)
{
	// update dynamic sky texture if/when needed
	if (bNeedToUpdateSky)
	{
		bNeedToUpdateSky = false;

		game->Context->ClearState();

		// Initialize sky draw state
		D3D11_VIEWPORT viewport = {};
		viewport.Width		= static_cast<float>(SKYDOME_TEXTURE_SIZE);
		viewport.Height		= static_cast<float>(SKYDOME_TEXTURE_SIZE);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;

		game->Context->RSSetViewports(1, &viewport);


		// Update constant buffers
		ps_sky_cbuffer PSCB;
		PSCB.g_sunIntensity = fSunIntensity;
		PSCB.g_sunDirection.x = Direction.x;
		PSCB.g_sunDirection.y = Direction.z;
		PSCB.g_sunDirection.z = Direction.y;
		PSCB.g_useDynamicSkyTexture = 1.0f;

		game->Context->UpdateSubresource(m_hPSCB, 0, nullptr, &PSCB, 0, 0);

		game->Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		game->Context->VSSetShader(m_hFullscreenVS, nullptr, 0);
		game->Context->PSSetShader(m_hUpdateSkyPS, nullptr, 0);

		game->Context->PSSetShaderResources(0, 1, &m_hStaticSkyDomeSRV);
		game->Context->PSSetSamplers(0, 1, &m_hBilinearSampler);
		game->Context->PSSetConstantBuffers(0, 1, &m_hPSCB);


		float factors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		game->Context->OMSetBlendState(blendState, factors, 0xFFFFFFFF);
		game->Context->OMSetDepthStencilState(depthState, 0);
		game->Context->RSSetState(rastState);

		game->Context->OMSetRenderTargets(1, &hDynamicSkyDomeRTV, nullptr);

		game->DebugAnnotation->BeginEvent(L"skyUpdate");
		game->Context->Draw(4, 0);
		game->DebugAnnotation->EndEvent();

		// Generatin mipmap chain
		//GenerateMipmaps();

		//game->Context->GenerateMips(hDynamicSkyDomeSRV);

		//DirectX::ScratchImage img;
		//DirectX::CaptureTexture(game->Device, game->Context, hDynamicSkyDomeTexture, img);
		//
		//DirectX::SaveToDDSFile(img.GetImages(), img.GetImageCount(), img.GetMetadata(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, L"skyTest.dds");
		//
		//img.Release();
	}
}


void SkyRenderer::Draw(float deltaTime)
{
	if (!bRenderSky) return;
	
	// Update constant buffers
	vs_sky_cbuffer VSCB;

	VSCB.g_matViewProj = Matrix::CreateTranslation(game->GameCamera->GetPosition()) * game->GameCamera->GetCameraMatrix();// game->GameCamera->ViewMatrix* Matrix::CreateInfinityProj(game->GameCamera->Fov, game->GameCamera->AspectRatio); // TODO: use infinite proj matrix
	game->Context->UpdateSubresource(m_hRenderSkyVSCB, 0, nullptr, &VSCB, 0, 0);


	ps_sky_cbuffer PSCB;
	PSCB.g_sunIntensity = fSunIntensity;
	PSCB.g_sunDirection = Direction;
	PSCB.g_useDynamicSkyTexture = 1.0f;
	game->Context->UpdateSubresource(m_hPSCB, 0, nullptr, &PSCB, 0, 0);


	// Rendering
	UINT stride = 24, offset = 0;
	game->Context->IASetVertexBuffers(0, 1, &m_hRenderSkyVertexBuffer, &stride, &offset);
	game->Context->IASetIndexBuffer(m_hRenderSkyIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	game->Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	game->Context->IASetInputLayout(m_hRenderSkyLayout);

	game->Context->VSSetShader(m_hRenderSkyVS, nullptr, 0);
	game->Context->PSSetShader(m_hRenderSkyPS, nullptr, 0);

	game->Context->VSSetConstantBuffers(0, 1, &m_hRenderSkyVSCB);
	game->Context->PSSetShaderResources(0, 1, &hDynamicSkyDomeSRV);
	game->Context->PSSetSamplers(0, 1, &m_hBilinearSampler);
	game->Context->PSSetConstantBuffers(0, 1, &m_hPSCB);

	//auto FB = getFramebuffer();

	//game->Context->OMSetRenderTargets(1, &FB, nullptr);

	//float factors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//game->Context->OMSetBlendState(blendState, factors, 0xFFFFFFFF);
	//game->Context->OMSetDepthStencilState(depthState, 0);
	//game->Context->RSSetState(rastState);

	game->DebugAnnotation->BeginEvent(L"skyDraw");
	game->Context->DrawIndexed(SKYDOME_SLICES_COUNT * (SKYDOME_SLICES_COUNT + 2) * 2, 0, 0);
	game->DebugAnnotation->EndEvent();
}


void SkyRenderer::BackBufferResizing()
{
	m_hRenderSkyFBs->Release();
}

void SkyRenderer::BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount)
{
	m_backBufferWidth = width;
	m_backBufferHeight = height;
	m_sampleCount = sampleCount;

	//if (m_pDevice == nullptr) return;
	//if (m_pState == nullptr) return;

	m_needToRecreatePSOs = true;
}


bool SkyRenderer::Init()
{
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = m_backBufferWidth;
	td.Height = m_backBufferHeight;
	td.ArraySize = 1;
	td.MipLevels = 1;
	td.SampleDesc.Count = m_sampleCount;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	game->Device->CreateTexture2D(&td, nullptr, &m_hInitTimeRenderTargetTexture);
	game->Device->CreateRenderTargetView(m_hInitTimeRenderTargetTexture, nullptr, &m_hInitTimeFB);


	// Load shaders
	ID3DBlob* vertexBC, *pixelBC;
	ID3DBlob* errorVertexCode;
	auto res = D3DCompileFromFile(L"./Shaders/SkyRenderingVS.hlsl", nullptr /*macros*/, nullptr /*include*/, "main", "vs_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexBC, &errorVertexCode);
	game->Device->CreateVertexShader(vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(), nullptr, &m_hRenderSkyVS);
	if (m_hRenderSkyVS == nullptr) return false;

	SafeRelease(errorVertexCode);

	res = D3DCompileFromFile(L"./Shaders/SkyRenderingPS.hlsl", nullptr /*macros*/, nullptr /*include*/, "main", "ps_5_0", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorVertexCode);
	game->Device->CreatePixelShader(pixelBC->GetBufferPointer(), pixelBC->GetBufferSize(), nullptr, &m_hRenderSkyPS);
	if (m_hRenderSkyPS == nullptr) return false;

	SafeRelease(errorVertexCode);
	SafeRelease(pixelBC);


	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 	0, D3D11_INPUT_PER_VERTEX_DATA, 0}, 
		D3D11_INPUT_ELEMENT_DESC {
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create input layout
	game->Device->CreateInputLayout(inputElements, 2, vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(), &m_hRenderSkyLayout);
	SafeRelease(vertexBC);

	// Create textures
	DirectX::ScratchImage img;
	DirectX::LoadFromDDSFile(L"Content/sky_dome_1.dds", DirectX::DDS_FLAGS_NONE, nullptr, img);
	DirectX::CreateTextureEx(game->Device,
		img.GetImages(), img.GetImageCount(), img.GetMetadata(),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE,
		0, 0, true,
		(ID3D11Resource**)&m_hStaticSkyDomeTexture);

	img.Release();
	if (m_hStaticSkyDomeTexture == nullptr) return false;

	game->Device->CreateShaderResourceView(m_hStaticSkyDomeTexture, nullptr, &m_hStaticSkyDomeSRV);

	td.Width = SKYDOME_TEXTURE_SIZE;
	td.Height = SKYDOME_TEXTURE_SIZE;
	td.SampleDesc.Count = 1;
	td.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.MipLevels = 1;
	td.MiscFlags = 0;
	auto hres = game->Device->CreateTexture2D(&td, nullptr, &hDynamicSkyDomeTexture);
	game->Device->CreateRenderTargetView(hDynamicSkyDomeTexture, nullptr, &hDynamicSkyDomeRTV);
	game->Device->CreateShaderResourceView(hDynamicSkyDomeTexture, nullptr, &hDynamicSkyDomeSRV);

	// Create samplers
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP; // Need it to avoid issues sampling the skydome texture in singularities in zenith and nadir points
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	game->Device->CreateSamplerState(&samplerDesc, &m_hBilinearSampler);

	samplerDesc.MaxAnisotropy = 16.0f;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	game->Device->CreateSamplerState(&samplerDesc, &m_hAnisotropicSampler);

	// Init constant buffer for vertex shader
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(vs_sky_cbuffer);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	game->Device->CreateBuffer(&cbDesc, nullptr, &m_hRenderSkyVSCB);

	// Init constant buffer for pixel shader
	cbDesc.ByteWidth = sizeof(ps_sky_cbuffer);
	game->Device->CreateBuffer(&cbDesc, nullptr, &m_hPSCB);

	// Create sky rendering PSO & bindings
	D3D11_BLEND_DESC blendStateDesc = {};
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendStateDesc.RenderTarget[0].BlendEnable = false;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	game->Device->CreateBlendState(&blendStateDesc, &blendState);


	D3D11_RASTERIZER_DESC rasterStateDesc = {};
	rasterStateDesc.FillMode = D3D11_FILL_SOLID;
	rasterStateDesc.CullMode = D3D11_CULL_NONE;
	rasterStateDesc.ScissorEnable	= false;
	rasterStateDesc.DepthClipEnable	= false;

	game->Device->CreateRasterizerState(&rasterStateDesc, &rastState);

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};
	depthStencilStateDesc.DepthEnable = false;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilStateDesc.StencilEnable = false;

	game->Device->CreateDepthStencilState(&depthStencilStateDesc, &depthState);


	// Create VB and IB for skydome
	uint32_t skydome_vertices_count = SKYDOME_SLICES_COUNT * (SKYDOME_SLICES_COUNT + 2) * 2;

	float* skydome_vertexdata = (float*)malloc(skydome_vertices_count * 6 * sizeof(float));
	uint32_t* skydome_indexdata = (uint32_t*)malloc(skydome_vertices_count * sizeof(uint32_t));

	for (uint32_t i = 0; i < skydome_vertices_count; i++) skydome_indexdata[i] = i;

	float skydome_radius = game->GameCamera->FarPlaneDistance * 0.8f;

	for (int j = 0; j < SKYDOME_SLICES_COUNT; j++)
	{
		int i = 0;
		int floatnum = (j * (SKYDOME_SLICES_COUNT + 2) * 2) * 6;
		skydome_vertexdata[floatnum + 0] = skydome_radius * cos(2.0f * PI * (float)i / (float)SKYDOME_SLICES_COUNT) * cos(-0.5f * PI + PI * (float)j / (float)SKYDOME_SLICES_COUNT);
		skydome_vertexdata[floatnum + 2] = skydome_radius * sin(-0.5f * PI + PI * (float)(j) / (float)SKYDOME_SLICES_COUNT);
		skydome_vertexdata[floatnum + 1] = skydome_radius * sin(2.0f * PI * (float)i / (float)SKYDOME_SLICES_COUNT) * cos(-0.5f * PI + PI * (float)j / (float)SKYDOME_SLICES_COUNT);
		skydome_vertexdata[floatnum + 3] = 1.0f;
		skydome_vertexdata[floatnum + 4] = (float)i / (float)SKYDOME_SLICES_COUNT;
		skydome_vertexdata[floatnum + 5] = (float)(j + 0) / (float)SKYDOME_SLICES_COUNT;
		floatnum += 6;
		for (i = 0; i < SKYDOME_SLICES_COUNT + 1; i++)
		{
			skydome_vertexdata[floatnum + 0] = skydome_radius * cos(2.0f * PI * (float)i / (float)SKYDOME_SLICES_COUNT) * cos(-0.5f * PI + PI * (float)j / (float)SKYDOME_SLICES_COUNT);
			skydome_vertexdata[floatnum + 2] = skydome_radius * sin(-0.5f * PI + PI * (float)(j) / (float)SKYDOME_SLICES_COUNT);
			skydome_vertexdata[floatnum + 1] = skydome_radius * sin(2.0f * PI * (float)i / (float)SKYDOME_SLICES_COUNT) * cos(-0.5f * PI + PI * (float)j / (float)SKYDOME_SLICES_COUNT);
			skydome_vertexdata[floatnum + 3] = 1.0f;
			skydome_vertexdata[floatnum + 4] = (float)i / (float)SKYDOME_SLICES_COUNT;
			skydome_vertexdata[floatnum + 5] = 1.0f * (float)(j + 0) / (float)SKYDOME_SLICES_COUNT;
			floatnum += 6;
			skydome_vertexdata[floatnum + 0] = skydome_radius * cos(2.0f * PI * (float)i / (float)SKYDOME_SLICES_COUNT) * cos(-0.5f * PI + PI * (float)(j + 1) / (float)SKYDOME_SLICES_COUNT);
			skydome_vertexdata[floatnum + 2] = skydome_radius * sin(-0.5f * PI + PI * (float)(j + 1) / (float)SKYDOME_SLICES_COUNT);
			skydome_vertexdata[floatnum + 1] = skydome_radius * sin(2.0f * PI * (float)i / (float)SKYDOME_SLICES_COUNT) * cos(-0.5f * PI + PI * (float)(j + 1) / (float)SKYDOME_SLICES_COUNT);
			skydome_vertexdata[floatnum + 3] = 1.0f;
			skydome_vertexdata[floatnum + 4] = (float)i / (float)SKYDOME_SLICES_COUNT;
			skydome_vertexdata[floatnum + 5] = 1.0f * (float)(j + 1) / (float)SKYDOME_SLICES_COUNT;
			floatnum += 6;
		}
		i = SKYDOME_SLICES_COUNT;
		skydome_vertexdata[floatnum + 0] = skydome_radius * cos(2.0f * PI * (float)i / (float)SKYDOME_SLICES_COUNT) * cos(-0.5f * PI + PI * (float)(j + 1) / (float)SKYDOME_SLICES_COUNT);
		skydome_vertexdata[floatnum + 2] = skydome_radius * sin(-0.5f * PI + PI * (float)(j + 1) / (float)SKYDOME_SLICES_COUNT);
		skydome_vertexdata[floatnum + 1] = skydome_radius * sin(2.0f * PI * (float)i / (float)SKYDOME_SLICES_COUNT) * cos(-0.5f * PI + PI * (float)(j + 1) / (float)SKYDOME_SLICES_COUNT);
		skydome_vertexdata[floatnum + 3] = 1.0f;
		skydome_vertexdata[floatnum + 4] = (float)i / (float)SKYDOME_SLICES_COUNT;
		skydome_vertexdata[floatnum + 5] = 1.0f * (float)(j + 1) / (float)SKYDOME_SLICES_COUNT;
	}

	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = skydome_vertices_count * 6 * sizeof(float);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = skydome_vertexdata;
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;

	game->Device->CreateBuffer(&bd, &initialData, &m_hRenderSkyVertexBuffer);


	bd.ByteWidth = skydome_vertices_count * sizeof(uint32_t);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	initialData.pSysMem = skydome_indexdata;
	game->Device->CreateBuffer(&bd, &initialData, &m_hRenderSkyIndexBuffer);

	free(skydome_vertexdata);
	free(skydome_indexdata);

	// Creating resources for updating sky

	// Shaders
	ID3DBlob* VSBinary, * PSBinary;
	res = D3DCompileFromFile(L"./Shaders/SkyUpdatePS.hlsl", nullptr /*macros*/, nullptr /*include*/, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &VSBinary, &errorVertexCode);
	game->Device->CreateVertexShader(VSBinary->GetBufferPointer(), VSBinary->GetBufferSize(), nullptr, &m_hFullscreenVS);
	SafeRelease(errorVertexCode);

	res = D3DCompileFromFile(L"./Shaders/SkyUpdatePS.hlsl", nullptr /*macros*/, nullptr /*include*/, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &PSBinary, &errorVertexCode);
	game->Device->CreatePixelShader(PSBinary->GetBufferPointer(), PSBinary->GetBufferSize(), nullptr, &m_hUpdateSkyPS);
	SafeRelease(errorVertexCode);
	SafeRelease(PSBinary);


	if (m_hFullscreenVS == nullptr) return false;
	if (m_hUpdateSkyPS == nullptr) return false;
	
	return true;
}

void SkyRenderer::Shutdown()
{
}

ID3D11RenderTargetView* SkyRenderer::getFramebuffer()
{
	D3D11_TEXTURE2D_DESC td;
	game->backBuffer->GetDesc(&td);

	// Reallocate the depth buffer if needed
	if (m_hRenderSkyFBs == nullptr) {
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width	= td.Width;
		desc.Height	= td.Height;
		desc.SampleDesc.Count= td.SampleDesc.Count;
		desc.Format = td.Format;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.ArraySize = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;

		game->Device->CreateTexture2D(&desc, nullptr, &m_hRenderSkyTex);
		game->Device->CreateRenderTargetView(m_hRenderSkyTex, nullptr, &m_hRenderSkyFBs);
	}

	return m_hRenderSkyFBs;
}



