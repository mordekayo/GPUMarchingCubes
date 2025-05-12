#pragma once
#include "GameComponent.h"
#include "Exports.h"


class Game;
class Camera;


class GAMEFRAMEWORK_API LightComponent : public GameComponent
{
	ID3D11Texture2D* resource;
	ID3D11DepthStencilView* DepthStencilView;
	
	ID3D11VertexShader* vertexShader;
	ID3DBlob* vertexShaderByteCode;
	ID3D11Buffer* constantBuffer;

	ID3D11RasterizerState* rastState;

	Camera* camera;

	DirectX::SimpleMath::Matrix ViewProjMatrix; 

public:
	ID3D11ShaderResourceView* DepthShaderView;

	int ShadowMapSize = 4096;

	DirectX::SimpleMath::Vector3 Position;
	DirectX::SimpleMath::Vector3 Direction;
	DirectX::SimpleMath::Vector3 Up;

	DirectX::SimpleMath::Matrix GetViewProjMatrix() const;
	
public:
	LightComponent(Game* game);
	virtual ~LightComponent() override;
	virtual void Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void Draw(float deltaTime) override;
	virtual void DestroyResources() override;
	virtual void Reload() override;

	void ReCreateRasterizerState(int DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias);
};

