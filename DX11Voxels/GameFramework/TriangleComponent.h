#pragma once
#include "GameComponent.h"
#include "Exports.h"

class Game;
class Camera;

class GAMEFRAMEWORK_API TriangleComponent : public GameComponent
{
public:
	ID3D11PixelShader*	pixelShader		= nullptr;
	ID3D11VertexShader*	vertexShader	= nullptr;
	ID3DBlob*	pixelShaderByteCode		= nullptr;
	ID3DBlob*	vertexShaderByteCode	= nullptr;
	ID3D11InputLayout*	layout			= nullptr;
	ID3D11Buffer*	vertices			= nullptr;
	ID3D11Buffer*	constantBuffer		= nullptr;
	ID3D11RasterizerState* rastState	= nullptr;
	ID3DUserDefinedAnnotation* annotation = nullptr;

	Camera* camera = nullptr;

	DirectX::SimpleMath::Vector4* points = nullptr;

	DirectX::SimpleMath::Vector3 Position;

	TriangleComponent(Game* inGame, Camera* inCamera);
	
	virtual void Initialize()				override;
	virtual void Update(float deltaTime)	override;
	virtual void Draw(float deltaTime)		override;
	virtual void DestroyResources()			override;
};

