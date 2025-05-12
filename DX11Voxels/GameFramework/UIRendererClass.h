#pragma once

#include "Exports.h"
#include <d3d11.h>
#include <map>
#include <vector>

struct ImDrawData;
class Game;


class GAMEFRAMEWORK_API UIRendererClass
{
	Game* game;


	std::map<void*, ID3D11ShaderResourceView*> loadedTextures;

	ID3D11Texture2D*			FontTexture;
	ID3D11ShaderResourceView*	FontSrv;

public:
	UIRendererClass(Game* inGame);
	~UIRendererClass();

	void RebuildFontAtlas();


	void BeforeLayout(float gameTime);
	void AfterLayout();

public:
	void ImGuiLayout();

public:
	static LRESULT WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
