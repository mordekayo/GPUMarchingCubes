#pragma once
#include "Exports.h"



struct GAMEFRAMEWORK_API VertexPosition2DTex2DColor
{
	DirectX::SimpleMath::Vector2 Position;
	DirectX::SimpleMath::Vector2 Tex;
	DirectX::SimpleMath::Color Color;

	static const int Stride = 20;

	static ID3D11InputLayout* GetLayout(ID3DBlob* signature);
};

