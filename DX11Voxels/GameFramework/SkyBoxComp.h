#pragma once

#include "Exports.h"
#include "Game.h"


class GAMEFRAMEWORK_API SkyBoxGameComponent : public GameComponent
{
	bool isInitialized = false;
public:
	ID3D11Texture2D* cubeMap;
	ID3D11ShaderResourceView* SRV;
	ID3D11Buffer* vertexBuffer;

	int vertsCount;

	struct ConstData
	{
		DirectX::SimpleMath::Matrix World;
		DirectX::SimpleMath::Matrix View;
		DirectX::SimpleMath::Matrix Proj;
	};

	ID3D11Buffer* constBuf;
	ConstData cData;

	Camera* cam;

	ID3D11VertexShader*	vs;
	ID3D11PixelShader*	ps;

	ID3D11InputLayout* layout;

	ID3D11RasterizerState*	rastState;
	ID3D11SamplerState*		sampler;

public:
	SkyBoxGameComponent(Game* game, Camera* cam);

	void Initialize() override;

	void Update(float deltaTime) override;
	void Draw(float deltaTime) override;

	void DestroyResources() override;
};

