#pragma once

#include "../GameFramework/Game.h"


class ReflectionSphereComp : public GameComponent
{
	ID3D11ShaderResourceView*	CubeMapSRV		= nullptr;
	ID3D11Buffer*				VertexBuffer	= nullptr;
	UINT VertsCount = 0;

	struct ConstData
	{
		DirectX::SimpleMath::Matrix World;
		DirectX::SimpleMath::Matrix View;
		DirectX::SimpleMath::Matrix Proj;
		DirectX::SimpleMath::Vector3 ViewerPosition;
		float Dummy;
	};

	ID3D11Buffer* constBuf = nullptr;
	ConstData cData {};

	Camera* cam = nullptr;

	ID3D11VertexShader*	vs = nullptr;
	ID3D11PixelShader*	ps = nullptr;

	ID3D11InputLayout* layout = nullptr;

	ID3D11RasterizerState*	rastState	= nullptr;
	ID3D11SamplerState*		sampler		= nullptr;


public:
	ReflectionSphereComp(Game* game, Camera* cam);


	void DestroyResources() override;

	void Draw(float deltaTime) override;

	void Initialize() override;

	void Update(float deltaTime) override;
};

