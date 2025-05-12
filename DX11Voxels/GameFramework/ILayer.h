#pragma once

#include "Exports.h"
#include "Game.h"


class GAMEFRAMEWORK_API ILayer
{
public:
	virtual void Update(float dt) = 0;	
	virtual void Draw(float dt) = 0;

	virtual void Init() = 0;
	virtual void Destroy() = 0;


	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
};

class GAMEFRAMEWORK_API IGameLayer : public ILayer
{
public:


};


class GAMEFRAMEWORK_API IEditorLayer : public ILayer
{
public:


};





class GAMEFRAMEWORK_API ZEditorLayer : public IEditorLayer
{
	ID3D11Texture2D* backBuffer = nullptr;
	ID3D11Texture2D* depthBuffer = nullptr;

	ID3D11RenderTargetView* RenderView = nullptr;
	ID3D11DepthStencilView* DepthView = nullptr;

	ID3D11ShaderResourceView* RenderSRV = nullptr;
	
public:


};

