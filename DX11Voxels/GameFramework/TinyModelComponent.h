#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "Exports.h"


class LightComponent;
struct TinyMaterial;
struct TinyShape;

namespace TinySpace
{
	extern GAMEFRAMEWORK_API float GlobalDepthOffset;
}

class GAMEFRAMEWORK_API TinyModelComponent : public GameComponent
{
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader*	pixelShader;
	ID3D11VertexShader* shadowShader;
	ID3DBlob*	pixelShaderByteCode;
	ID3DBlob*	vertexShaderByteCode;
	ID3DBlob*	shadowByteCode;
	ID3D11Buffer*			constantBuffer;
	ID3D11Buffer*			constantDepthBuffer;
	ID3D11Buffer*			lightBuffer;
	ID3D11RasterizerState*	rastState;
	ID3D11BlendState*		blendState;

	ID3D11SamplerState* sampler;
	ID3D11SamplerState* depthSampler;


	ID3D11Buffer* indexBuffer;

	ID3D11Buffer* vBuf = nullptr;
	ID3D11Buffer* nBuf = nullptr;
	ID3D11Buffer* tBuf = nullptr;
	ID3D11Buffer* strBuf = nullptr;

	ID3D11ShaderResourceView* vSrv = nullptr;
	ID3D11ShaderResourceView* nSrv = nullptr;
	ID3D11ShaderResourceView* tSrv = nullptr;
	ID3D11ShaderResourceView* strSrv = nullptr;
	
	TinyMaterial*	Materials;
	TinyShape*		Shapes;

	Camera*	camera;
	
	const char* modelName = "";
	int elemCount = 0;


public:
	DirectX::SimpleMath::Vector3	Position	= DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Matrix		Transform	= DirectX::SimpleMath::Matrix::Identity;

	void DrawToDepth(LightComponent* light);


	LightComponent* MainLight;
	
public: 
	TinyModelComponent(Game* inGame, Camera* inCamera, LightComponent* inLight, const char* inFileName);
	
	virtual void Initialize() override; 
	virtual void Update(float deltaTime) override;
	virtual void Draw(float deltaTime) override;
	virtual void DestroyResources() override;
};

