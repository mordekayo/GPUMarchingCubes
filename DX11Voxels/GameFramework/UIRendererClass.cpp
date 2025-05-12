#include "UIRendererClass.h"
#include "ImGUI/imgui.h"
#include "../ZMathLib/SimpleMath.h"
#include "Game.h"
#include "VertexPositon2DTex2DColor.h"
#include "windowsx.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImPlot/implot.h"


using namespace DirectX::SimpleMath;


UIRendererClass::UIRendererClass(Game* inGame)
{
	game = inGame;

	ImGui_ImplWin32_EnableDpiAwareness();

	auto imCntx = ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); //(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI
	
	ImGui_ImplWin32_Init(game->Display->hWnd);
	ImGui_ImplDX11_Init(game->Device, game->Context);

	ImGui::StyleColorsDark();
}


UIRendererClass::~UIRendererClass()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ImPlot::DestroyContext();
}


void UIRendererClass::RebuildFontAtlas()
{
	// Get font texture from ImGui
	auto& io = ImGui::GetIO();
	//io.Fonts.AddFontDefault();
	//io.Fonts.AddFontFromFileTTF("./content/fonts/arial.ttf", 8, null, io.Fonts.GetGlyphRangesCyrillic());
	byte* pixelData;
	int width;
	int height;
	int bytesPerPixel;
	io.Fonts->GetTexDataAsRGBA32(&pixelData, &width, &height, &bytesPerPixel);

	// Create and register the texture as an XNA texture
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.MiscFlags = 0;
	texDesc.MipLevels = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem				= pixelData;
	subData.SysMemPitch			= width * bytesPerPixel;
	subData.SysMemSlicePitch	= subData.SysMemPitch * height;
	
	game->Device->CreateTexture2D(&texDesc, &subData, &FontTexture);
	
	//game->Context->UpdateSubresource(FontTexture, 0, nullptr, pixelData, width* bytesPerPixel, 0);

	game->Device->CreateShaderResourceView(FontTexture, nullptr, &FontSrv);

	// Let ImGui know where to find the texture
	io.Fonts->SetTexID(FontSrv);
	io.Fonts->ClearTexData(); // Clears CPU side texture data
}


void UIRendererClass::BeforeLayout(float gameTime)
{
	//ImGui::GetIO().DisplaySize = ImVec2(game->ClientWidth, game->ClientHeight);
	//ImGui::GetIO().DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	auto& io = ImGui::GetIO();
	io.DeltaTime = gameTime;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}


void UIRendererClass::AfterLayout()
{
	ImGui::Render();
	
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}



bool show_test_window = false;
bool show_another_window = false;
float f = 1;

void UIRendererClass::ImGuiLayout()
{
	// 1. Show a simple window
			// Tip: if we don't call ImGui.Begin()/ImGui.End() the widgets appears in a window automatically called "Debug"
	{
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		if (ImGui::Button("Test Window")) show_test_window = !show_test_window;
		if (ImGui::Button("Another Window")) show_another_window = !show_another_window;

		ImGui::Text("Texture sample");
		//ImGui.Image(_imGuiTexture, new Num.Vector2(300, 150), Num.Vector2.Zero, Num.Vector2.One, Num.Vector4.One, Num.Vector4.One); // Here, the previously loaded texture is used
	}

	// 2. Show another simple window, this time using an explicit Begin/End pair
	if (show_another_window)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello");
		ImGui::End();
	}

	// 3. Show the ImGui test window. Most of the sample code is in ImGui.ShowTestWindow()
	if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::ShowDemoWindow(&show_test_window);
	}
}



// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT UIRendererClass::WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		return true;
	}
	
	return 0;
}