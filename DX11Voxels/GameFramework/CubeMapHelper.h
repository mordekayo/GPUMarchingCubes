#pragma once
#include "Exports.h"
#include <d3d11.h>

class Game;

struct ZTexViews
{
	ID3D11Texture2D*			Tex = nullptr;
	ID3D11RenderTargetView*		Rtv = nullptr;
	ID3D11ShaderResourceView*	Srv = nullptr;
};

struct GAMEFRAMEWORK_API CubeMapHelper
{
	ID3D11VertexShader*		vsGenCube = nullptr;
	ID3D11GeometryShader*	gsGenCube = nullptr;
	ID3D11PixelShader*		psSphericalToCube = nullptr;
	ID3D11PixelShader*		psPreFilterCube = nullptr;

	ID3D11Buffer* PreFilterConstBuf = nullptr;

	ID3D11VertexShader*		vsFullscreenPlane = nullptr;
	ID3D11PixelShader*		psCubeToPlanar = nullptr;
	ID3D11PixelShader*		psIntegrateBRDF = nullptr;

	ID3D11SamplerState* sampler = nullptr;
	ID3D11RasterizerState* rastState = nullptr;

	ID3D11PixelShader*		psConv = nullptr;

	D3D11_VIEWPORT viewport;
 
	const Game* game = nullptr;
	
	void Init(const Game* game);
	void ReloadShaders();

	void Convert(ID3D11ShaderResourceView* sphereMap, ZTexViews& outCubeMap);
	void ConvertHDR(LPCWSTR filePath, ZTexViews& outCubeMap, bool saveResult = false);

	void ConvertCubeToPlanar(const ZTexViews& inCubeMap, ID3D11ShaderResourceView*& outSphereMap, bool saveResult = false);
	
	void CalculateConvolutionMap(const ZTexViews& sourceMap, ZTexViews& outCubeMap);

	void PreFilterMipMaps(const ZTexViews& sourceMap, ZTexViews& outCubeMap);
	void GenerateIntegratedBRDFMap(ZTexViews& outMap);
	
	void LoadFromDDS(LPCWSTR filePath, ZTexViews& outCubeMap);

	void SaveToDDS(ID3D11Texture2D* tex, LPCWSTR path);
};



