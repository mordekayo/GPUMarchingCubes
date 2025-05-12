#pragma once
#include <map>
#include <vector>

#include "ShadersFlags.h"


namespace DirectX
{
	namespace SimpleMath
	{
		struct Vector3;
	}
}

class Camera;
struct ID3D11Texture2D;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct D3D11_VIEWPORT;


struct CascadedDepthRenderer
{
	ID3D11Texture2D*			shadowTexArr = nullptr;
	ID3D11DepthStencilView*		depthDSV = nullptr;
	ID3D11ShaderResourceView*	depthSRV = nullptr;
	ID3D11Buffer*	constBuf	= nullptr;
	ID3D11Buffer*	cascadeBuf	= nullptr;
	D3D11_VIEWPORT* viewport	= nullptr;

	ID3D11RasterizerState* rastState = nullptr;

	std::vector<VertexFlagsOpaque> vsFlagsDepth = {
		VertexFlagsOpaque::NONE,
		VertexFlagsOpaque::SKINNED_MESH
	};

	std::map<VertexFlagsOpaque, ID3D11VertexShader*> depthVSs;
	ID3D11GeometryShader* depthGS = nullptr;

	int width	= 1024;
	int height	= 1024;

public:
	CascadedDepthRenderer(int inWidth, int inHeight);

	void Update(Camera* inCamera, DirectX::SimpleMath::Vector3 inLightDirection);
};

