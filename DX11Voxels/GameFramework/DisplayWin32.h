#pragma once
#include "pch.h"

class DisplayWin32
{
public:
	DisplayWin32(std::wstring& windowName, int width, int height, WNDPROC wndProc);
	~DisplayWin32();
	
public:
	int ClientWidth = 0;
	int ClientHeight = 0;

#pragma region Window stuff
public:
	HINSTANCE	hInstance = nullptr;
	HWND		hWnd = nullptr;
	WNDCLASSEX	wc = {};

	static HMODULE Module;

#pragma endregion Window stuff
};

