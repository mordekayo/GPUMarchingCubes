#pragma once

#include "Exports.h"

#include "d3d11.h"
#include <string>


class GAMEFRAMEWORK_API IRenderer2D
{
public:
	virtual void Initialize() = 0;
	virtual void UnloadResources() = 0;
	virtual void SetTarget(ID3D11RenderTargetView* rtv) = 0;
	virtual void DrawSomething() = 0;
	virtual void DrawOnScreenMessage(std::wstring& message) = 0;
	virtual void Draw() = 0;
	virtual void ClearMessages() = 0;

	//virtual void ClearMessages() = 0;
};
