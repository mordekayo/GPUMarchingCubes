#pragma once

#include "Exports.h"

// Third party
#include "pch.h"
#include "GameComponent.h"
#include "../FbxLoader/FbxLoader.h"
#include "DisplayWin32.h"
#include "QueryGpuProfiler.h"
#include "ShaderIncludeHandler.h"


namespace FbxNative {
	class MeshVertex;
}

class InputDevice;
class TextureLoader;
class ObjLoader;
class Camera;
class DebugRenderSystem;
class UIRendererClass;
class IRenderer2D;
class ScriptingSystem;



namespace ZGame
{
	class CSharpScriptingSystem;
	class SoundSystemFMod;
}


class GAMEFRAMEWORK_API Game
{
public:

	static Game* Instance;

	std::wstring* Name;

	std::vector<GameComponent*> Components;
	MulticastDelegate<const DirectX::SimpleMath::Vector2&> ScreenResized;

	DisplayWin32* Display = nullptr;

	
#pragma region All the DirectX stuff
public:

	ID3D11Device*			Device		= nullptr;
	ID3D11Device2*			Device_2	= nullptr;
	IDXGISwapChain*			SwapChain	= nullptr;
	IDXGISwapChain1*		SwapChain1	= nullptr;
	ID3D11DeviceContext*	Context		= nullptr;
	ID3D11DeviceContext2*	Context_2	= nullptr;

	
	ID3D11Texture2D* backBuffer		= nullptr;
	ID3D11Texture2D* depthBuffer	= nullptr;

	ID3D11RenderTargetView* RenderView	= nullptr;
	ID3D11DepthStencilView* DepthView	= nullptr;

	ID3D11ShaderResourceView* RenderSRV = nullptr;
	

	ID3D11RasterizerState* RastState;
	ID3D11DepthStencilState* DepthState;

	D3D11_VIEWPORT ScreenViewport;



	//////// States
	ID3D11DepthStencilState* DepthStateNoDepth;

	ID3D11RasterizerState* RastStateCullBack;
	ID3D11RasterizerState* RastStateCullFront;

	ID3D11BlendState* BlendStateOpaque;


private:
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	
#pragma endregion All the DirectX stuff

#pragma region Helpers/Loaders
public:
	TextureLoader* TextureLoader;
	ObjLoader* ObjLoader;
	ID3DUserDefinedAnnotation* DebugAnnotation;
	FbxNative::FbxLoader* FbxLoader;
	ShaderIncludeHandler* IncludeHandler;
#pragma endregion Helpers/Loaders

#pragma region Systems
public:
	InputDevice*		InputDevice;
	DebugRenderSystem*	DebugRender;
	UIRendererClass*	UIRender;
	IRenderer2D*		Renderer2D;
	ScriptingSystem*	ScriptingEngine;
	ZGame::CSharpScriptingSystem* CSharpScriptingSys;
	ZGame::SoundSystemFMod*	SoundSystem;

	struct PhysicsSystemPhysX*	PhysicsSystem;
	struct PhysicsSystemJolt*	JoltPhysSystem;

	ZGame::QueryGpuProfiler* GpuProfiler;

	class NavigationSystem*		NavSystem;
	class CrowdTool*			crowdTool;
#pragma endregion Systems
	
public:
	std::chrono::time_point<std::chrono::steady_clock>* StartTime	= nullptr;
	std::chrono::time_point<std::chrono::steady_clock>* PrevTime	= nullptr;
	double TotalTime = 0;
	float DeltaTime = 0;
	uint64_t frameIndex = -1;

	DirectX::SimpleMath::Color ClearColor;

	Camera* GameCamera = nullptr;

	bool isExitRequested	= false;
	bool isActive			= false;
	bool isVSyncEnabled		= true;

public:
	Game(std::wstring& AppName);
	virtual ~Game();
	
	void Run(int WindowWidth, int WindowHeight);
	void Exit();

	void RestoreTargets() const;

	bool IsMainWindowActive() const;

	virtual LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	
protected:
	void PrepareResources();
	void UpdateInternal();
	void PrepareFrame();
	void EndFrame();
	void DestroyResources();

	void InitializeSubsystems();

	virtual void Initialize();
	virtual void PostDraw(float deltaTime);
	virtual void Update(float deltaTime);
	virtual void Draw(float deltaTime);

private:
	void CreateBackBuffer();
};

// std::wstring stemp = std::wstring(s.begin(), s.end());
// LPCWSTR sw = stemp.c_str();


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


template<class T>
void SafeRelease(T*& obj)
{
	if(obj) {
		obj->Release();
		obj = nullptr;
	}
}


GAMEFRAMEWORK_API void CompileShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR target, ID3DBlob*& shCode, bool useDebug);
