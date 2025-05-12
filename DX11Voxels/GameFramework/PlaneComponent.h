#pragma once
#include "GameComponent.h"
#include "Exports.h"

class Game;
class Camera;

class GAMEFRAMEWORK_API PlaneComponent : public GameComponent
{
	ID3D11PixelShader*	pixelShader;
	ID3D11VertexShader* vertexShader;
	ID3DBlob* pixelShaderByteCode;
	ID3DBlob* vertexShaderByteCode;
	ID3D11InputLayout* layout;
	ID3D11Buffer* vertices;
	ID3D11Buffer* constantBuffer;
	ID3D11RasterizerState* rastState;
	Camera* camera;

	std::vector<DirectX::SimpleMath::Vector4>* points = nullptr;

	
public:

	PlaneComponent(Game* inGame, Camera* inCamera);
	
	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw(float deltaTime) override;
	void DestroyResources() override;
};

