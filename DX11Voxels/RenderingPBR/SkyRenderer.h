#pragma once

#include "Game.h"


#define SKYDOME_SLICES_COUNT 50
#define SKYDOME_TEXTURE_SIZE 2048
#define SKYDOME_TEXTURE_MIPMAPCOUNT 12

///*
class SkyRenderer
{
	Game* game = nullptr;

public:
	bool bNeedToUpdateSky = true;
	DirectX::SimpleMath::Vector3 Direction = DirectX::SimpleMath::Vector3(0.57f, 0.57f, 0.57f);


	SkyRenderer(Game* inGame);

	// IRenderPass interface
	void Update(float deltaTime);
	void Draw(float deltaTime);
	void BackBufferResizing();
	void BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount);

	// Additional funcs
	bool Init();
	void Shutdown();
	//void UpdateSkyTexture();

private:
	ID3D11RenderTargetView* getFramebuffer();

	// 3D objects
	ID3D11InputLayout* m_hRenderSkyLayout;
	ID3D11Buffer* m_hRenderSkyVertexBuffer;
	ID3D11Buffer* m_hRenderSkyIndexBuffer;

	ID3D11Texture2D* m_hStaticSkyDomeTexture;
	ID3D11ShaderResourceView* m_hStaticSkyDomeSRV;

	ID3D11Texture2D* hDynamicSkyDomeTexture;
	ID3D11RenderTargetView* hDynamicSkyDomeRTV;
	ID3D11ShaderResourceView* hDynamicSkyDomeSRV;

	ID3D11SamplerState* m_hBilinearSampler;
	ID3D11SamplerState* m_hAnisotropicSampler;

	ID3D11VertexShader* m_hRenderSkyVS;
	ID3D11PixelShader* m_hRenderSkyPS;
	ID3D11Buffer* m_hRenderSkyVSCB;
	ID3D11Buffer* m_hPSCB;

	ID3D11Texture2D* m_hRenderSkyTex		= nullptr;
	ID3D11RenderTargetView* m_hRenderSkyFBs = nullptr; // one entry per swap chain back buffer
	ID3D11RenderTargetView* m_hInitTimeFB;	      // Framebuffer required at init time to create PSO
	ID3D11Texture2D* m_hInitTimeRenderTargetTexture; // Render target texture required at init time to create PSO

	ID3D11InputLayout* m_hUpdateSkyLayout;
	ID3D11VertexShader* m_hFullscreenVS;
	ID3D11PixelShader* m_hUpdateSkyPS;
	//ID3D11RenderTargetView* m_hUpdateSkyFB;
	ID3D11Buffer* m_hUpdateSkyVertexBuffer;

	ID3D11BlendState* blendState;
	ID3D11RasterizerState* rastState;
	ID3D11DepthStencilState* depthState;

	DirectX::SimpleMath::Matrix m_matView;
	DirectX::SimpleMath::Matrix m_matProj;
	DirectX::SimpleMath::Matrix m_matMVP;

	uint32_t m_backBufferWidth = 1280;
	uint32_t m_backBufferHeight = 720;
	uint32_t m_sampleCount = 1;
	bool m_needToRecreatePSOs = false;


	float fSunIntensity = 0.5f;
	float fSunAngle = 45.0f;
	bool bRenderSky = true;
};
//*/

