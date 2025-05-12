#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include "pch.h"
#include "Game.h"

#include <filesystem>
#include <iostream>
#include "InputDevice.h"
#include "TextureLoader.h"
#include "ObjLoader.h"
#include "Camera.h"
#include "DebugRenderSysImpl.h"
#include "UIRendererClass.h"
#include "Renderer2DImpl.h"
#include "ScriptingSystem.h"
#include <ImGUI/imgui.h>

#include "NavigationSystem.h"
#include "CrowdTool.h"
#include "PhysicsSystemPhysX.h"
#include "PhysicsSystemJolt.h"
#include "SoundSystemFMod.h"


#include "CSharpScriptingSystem.h"


Game* Game::Instance = nullptr;

using namespace DirectX::SimpleMath;


//std::map<int, ID3D11Query*> timeQueries;
struct QueryBuf {
	ID3D11Query* queryDisjoint;
	ID3D11Query* queryAtStart;
	ID3D11Query* queryAtEnd;
};
QueryBuf qBuf;
QueryBuf qBufSecond;
QueryBuf* qBufCur;

void CollectTimestamps(ID3D11DeviceContext* pContext, QueryBuf* buf)
{
	// Wait for data to be available
	while (pContext->GetData(buf->queryDisjoint, NULL, 0, 0) == S_FALSE)
	{
		Sleep(1);       // Wait a bit, but give other threads a chance to run
	}

	// Check whether timestamps were disjoint during the last frame
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
	pContext->GetData(buf->queryDisjoint, &tsDisjoint, sizeof(tsDisjoint), 0);
	if (tsDisjoint.Disjoint)
	{
		return;
	}

	//// Get all the timestamps
	//UINT64 tsBeginFrame, tsShadowClear; // ... etc.
	//	pContext->GetData(pQueryBeginFrame, &tsBeginFrame, sizeof(UINT64), 0);
	//pContext->GetData(pQueryShadowClear, &tsShadowClear, sizeof(UINT64), 0);
	//// ... etc.
	//
	//// Convert to real time
	//float msShadowClear = float(tsShadowClear - tsBeginFrame) /
	//	float(tsDisjoint.Frequency) * 1000.0f;
	//// ... etc.
}


Game::Game(std::wstring& AppName) : ClearColor(0.0f, 0.0f, 0.0f, 1.0f)
{
	Name		= new std::wstring(AppName);
	Components	= std::vector<GameComponent*>();

	Instance = this;
}


Game::~Game()
{

}


void CompileShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR target, ID3DBlob*& shCode, bool useDebug)
{
	ID3DBlob* errorVertexCode = nullptr;

	UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	if (useDebug) flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	auto res = D3DCompileFromFile(fileName,
		nullptr,
		Game::Instance->IncludeHandler,
		entryPoint,
		target,
		flags,
		0,
		&shCode,
		&errorVertexCode
	);

	if (FAILED(res)) {
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else {
			std::wcout << L"Missing Shader File: " << fileName << std::endl;;
		}
		return;
	}
	SafeRelease(errorVertexCode);
}


void Game::InitializeSubsystems()
{
	TextureLoader	= new ::TextureLoader(this);
	FbxLoader		= new FbxNative::FbxLoader();
	ObjLoader		= new ::ObjLoader(this);
	IncludeHandler	= new ShaderIncludeHandler();

	ScriptingEngine		= new ScriptingSystem();
	CSharpScriptingSys	= new ZGame::CSharpScriptingSystem();

	PhysicsSystem = new PhysicsSystemPhysX();
	PhysicsSystem->InitPhysics(true);

	JoltPhysSystem = new PhysicsSystemJolt();
	JoltPhysSystem->Initialize();

	GpuProfiler = new ZGame::QueryGpuProfiler();

	NavSystem = new NavigationSystem();

	crowdTool = new CrowdTool();
	crowdTool->init(NavSystem);

	SoundSystem = new ZGame::SoundSystemFMod();
}



void Game::Run(int WindowWidth, int WindowHeight)
{
	std::cout << "Working directory: " << std::filesystem::current_path() << "\n\n";

	Display = new DisplayWin32(*Name, WindowWidth, WindowHeight, WndProc);
	
	PrepareResources();

	InitializeSubsystems();

	Initialize();
	for (auto com : Components) com->Initialize();

	StartTime	= new std::chrono::time_point<std::chrono::steady_clock>();
	PrevTime	= new std::chrono::time_point<std::chrono::steady_clock>();
	*StartTime	= std::chrono::steady_clock::now();
	*PrevTime	= *StartTime;
	
	TotalTime = 0;
	
	MSG msg = {};
	while (!isExitRequested)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			//if (msg.message == WM_QUIT)
			//{
			//	isExitRequested = true;
			//}
		}

		UpdateInternal();
	}

	delete StartTime;

	DestroyResources();
}

void Game::Exit()
{
	isExitRequested = true;
}


void Game::RestoreTargets() const
{
	Context->OMSetRenderTargets(1, &RenderView, DepthView);

	Context->RSSetViewports(1, &ScreenViewport);
}


bool Game::IsMainWindowActive() const
{
	return isActive && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}


void Game::PrepareResources()
{
	HRESULT res;

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width	= Display->ClientWidth;
	swapDesc.BufferDesc.Height	= Display->ClientHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = Display->hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;


	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };
	res = D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr,
#ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG |
#endif
		D3D11_CREATE_DEVICE_BGRA_SUPPORT, 
		featureLevel, 
		1, 
		D3D11_SDK_VERSION,
		&swapDesc, 
		&SwapChain, 
		&Device, 
		nullptr, 
		&Context);	ZCHECK(res);

	SwapChain->QueryInterface<IDXGISwapChain1>(&SwapChain1);
	Context->QueryInterface<ID3DUserDefinedAnnotation>(&DebugAnnotation);
	Context->QueryInterface<ID3D11DeviceContext2>(&Context_2);
	Device->QueryInterface<ID3D11Device2>(&Device_2);

	CreateBackBuffer();


	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	Device->CreateRasterizerState(&rastDesc, &RastState);

	
	DebugRender = new DebugRenderSysImpl(this);

	Renderer2D = new Renderer2DImpl();
	Renderer2D->Initialize();
	Renderer2D->SetTarget(RenderView);
	

	this->InputDevice = new ::InputDevice(this);

	GameCamera = new Camera(this);


	ScreenViewport.Width = static_cast<float>(Display->ClientWidth);
	ScreenViewport.Height = static_cast<float>(Display->ClientHeight);
	ScreenViewport.TopLeftX = 0;
	ScreenViewport.TopLeftY = 0;
	ScreenViewport.MinDepth = 0;
	ScreenViewport.MaxDepth = 1.0f;


	UIRender = new UIRendererClass(this);
	UIRender->RebuildFontAtlas();
	
	
	D3D11_QUERY_DESC qDesc = {};
	qDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	qDesc.MiscFlags = 0;
	
	Device->CreateQuery(&qDesc, &qBuf.queryDisjoint);



	auto depthDesc = D3D11_DEPTH_STENCIL_DESC{};
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.StencilEnable = false;
	depthDesc.StencilReadMask = 0x00;
	depthDesc.StencilWriteMask = 0x00;
	depthDesc.DepthEnable = false;
	Device->CreateDepthStencilState(&depthDesc, &DepthStateNoDepth);


	auto blendStateDesc = D3D11_BLEND_DESC{ false, false };
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	Device->CreateBlendState(&blendStateDesc, &BlendStateOpaque);


	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	Device->CreateRasterizerState(&rastDesc, &RastStateCullBack);

	rastDesc.CullMode = D3D11_CULL_FRONT;
	Device->CreateRasterizerState(&rastDesc, &RastStateCullFront);

}


void Game::UpdateInternal()
{
	++frameIndex;
	auto	curTime		= std::chrono::steady_clock::now();
	DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - *PrevTime).count() / 1000000.0f;
	*PrevTime = curTime;

	std::chrono::duration<double, std::milli> elapsedDouble = curTime - *StartTime;
	TotalTime = elapsedDouble.count() * 0.001;

	ScriptingEngine->lua["totalTime"] = TotalTime;
	ScriptingEngine->lua["dt"] = DeltaTime;

	SoundSystem->Update();

	PrepareFrame();

	//GpuProfiler->BeginFrame();

	//GpuProfiler->AddTimestamp("Update begin");
	Update(DeltaTime);
	//GpuProfiler->AddTimestamp("Update end");

	//GpuProfiler->AddTimestamp("Draw begin");
	Draw(DeltaTime);
	//GpuProfiler->AddTimestamp("Draw end");

	//GpuProfiler->AddTimestamp("DebugRender begin");
	RestoreTargets();
	if(DebugRender) DebugRender->Draw(DeltaTime);
	//GpuProfiler->AddTimestamp("DebugRender end");

	//GpuProfiler->AddTimestamp("PostDraw begin");
	PostDraw(DeltaTime);
	//GpuProfiler->AddTimestamp("PostDraw end");

	//GpuProfiler->EndFrame();
	EndFrame();
}


void Game::PrepareFrame()
{
	Context->ClearState();

	Context->OMSetRenderTargets(1, &RenderView, DepthView);

	Context->RSSetViewports(1, &ScreenViewport);
	Context->RSSetState(RastState);
	
	Context->ClearRenderTargetView(RenderView, ClearColor);
	Context->ClearDepthStencilView(DepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


void Game::EndFrame()
{
	Context->OMSetRenderTargets(0, nullptr, nullptr);
	//SwapChain->Present(1, 0);
	SwapChain1->Present(isVSyncEnabled, 0);
	//SwapChain1->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
}


void Game::DestroyResources()
{
	backBuffer->Release();
	delete TextureLoader;
	delete ObjLoader;
	delete GameCamera;
	delete InputDevice;
}


void Game::Initialize()
{
}


void Game::PostDraw(float deltaTime)
{
}


void Game::Update(float deltaTime)
{
	for (auto cmp : Components) cmp->Update(deltaTime);
}


void Game::Draw(float deltaTime)
{
	for (auto cmp : Components) cmp->Draw(deltaTime);
}


void Game::CreateBackBuffer()
{
	if(backBuffer	!= nullptr) backBuffer->Release();
	if(RenderView	!= nullptr) RenderView->Release();
	if(depthBuffer	!= nullptr) depthBuffer->Release();
	if(DepthView	!= nullptr) DepthView->Release();

	if (RenderSRV != nullptr) RenderSRV->Release();
	
	auto	res	= SwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)& backBuffer);	ZCHECK(res);
			res			= Device->CreateRenderTargetView(backBuffer, nullptr, &RenderView);		ZCHECK(res);
	
	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.ArraySize = 1;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.Width	= Display->ClientWidth;
	depthTexDesc.Height = Display->ClientHeight;
	depthTexDesc.SampleDesc = { 1, 0 };

	res = Device->CreateTexture2D(&depthTexDesc, nullptr, &depthBuffer); ZCHECK(res);
	

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStenDesc = {};
	depthStenDesc.Format		= DXGI_FORMAT_D32_FLOAT;
	depthStenDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStenDesc.Flags = 0;

	res = Device->CreateDepthStencilView(depthBuffer, &depthStenDesc, &DepthView); ZCHECK(res);

	//D3D11_DEPTH_STENCIL_DESC depthStateDesc;
	//res = Device->CreateDepthStencilState(&depthStateDesc, &DepthState);
}


LRESULT Game::MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	UIRendererClass::WndProcHandler(hwnd, umessage, wparam, lparam);
	
	switch (umessage)
	{
		case WM_DESTROY:
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			isExitRequested = true;
			return 0;
		}
		
		case WM_ENTERSIZEMOVE:
		{
			return 0;
		}

		case WM_EXITSIZEMOVE:
		{
			return 0;
		}


		case WM_ACTIVATEAPP:
		{
			isActive = (bool)wparam;

			return 0;
		}

		
		case WM_INPUT:
		{
			UINT dwSize = 0;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == nullptr) {
				return 0;
			}

			if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
				OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				//printf(" Kbd: make=%04i Flags:%04i Reserved:%04i ExtraInformation:%08i, msg=%04i VK=%i \n",
				//	raw->data.keyboard.MakeCode,
				//	raw->data.keyboard.Flags,
				//	raw->data.keyboard.Reserved,
				//	raw->data.keyboard.ExtraInformation,
				//	raw->data.keyboard.Message,
				//	raw->data.keyboard.VKey);

				InputDevice->OnKeyDown({
					raw->data.keyboard.MakeCode,
					raw->data.keyboard.Flags,
					raw->data.keyboard.VKey,
					raw->data.keyboard.Message
				});
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				//printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				InputDevice->OnMouseMove({
					raw->data.mouse.usFlags,
					raw->data.mouse.usButtonFlags,
					static_cast<int>(raw->data.mouse.ulExtraInformation),
					static_cast<int>(raw->data.mouse.ulRawButtons),
					static_cast<short>(raw->data.mouse.usButtonData),
					raw->data.mouse.lLastX,
					raw->data.mouse.lLastY
				});
			}
			else if (raw->header.dwType == RIM_TYPEHID)
			{
				LPSTR str = new char[100];
				UINT pDataSize = 0;
				GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICENAME, str, &pDataSize);
				//OutputDebugString(str);
				delete[] str;

			}

			delete[] lpb;
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}

		case WM_SIZE:
		{
			std::cout << "Width " << LOWORD(lparam) << " Height " << HIWORD(lparam) << std::endl;

			
			if (Device) {

				int newWidth = LOWORD(lparam);
				int newHeight = HIWORD(lparam);

				if(newWidth == 0 || newHeight == 0 || (Display->ClientWidth == newWidth && Display->ClientHeight == newHeight)) {
					return 0;
				}

				ScreenViewport.Width	= static_cast<float>(newWidth);
				ScreenViewport.Height	= static_cast<float>(newHeight);
				ScreenViewport.TopLeftX = 0;
				ScreenViewport.TopLeftY = 0;
				ScreenViewport.MinDepth = 0;
				ScreenViewport.MaxDepth = 1.0f;
				
				Display->ClientWidth	= newWidth;
				Display->ClientHeight	= newHeight;

				if (backBuffer != nullptr) {
					backBuffer->Release(); backBuffer = nullptr;
				}
				if (RenderView != nullptr) {
					RenderView->Release(); RenderView = nullptr;
				}
				if (depthBuffer != nullptr) {
					depthBuffer->Release(); depthBuffer = nullptr;
				}
				if (DepthView != nullptr) {
					DepthView->Release(); DepthView = nullptr;
				}

				Renderer2D->UnloadResources();

				SwapChain1->ResizeBuffers(2, Display->ClientWidth, Display->ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
				
				CreateBackBuffer();

				Renderer2D->SetTarget(RenderView);

				const auto screenSize = Vector2{ static_cast<float>(Display->ClientWidth), static_cast<float>(Display->ClientHeight) };
				ScreenResized.Broadcast(screenSize);
		}
			
		return 0;
	}

		//case WM_MOVING:
		//{
		//	//std::cout << "Moving " << std::endl;
		//	//Game::Instance->UpdateInternal();
		//	return 0;
		//}

		// All other messages pass to the message handler in the system class.
		default:
			return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	return Game::Instance->MessageHandler(hwnd, umessage, wparam, lparam);
	
	//switch (umessage)
	//{
	//	default:
	//	{
	//		return Game::Instance->MessageHandler(hwnd, umessage, wparam, lparam);
	//	}
	//}
}