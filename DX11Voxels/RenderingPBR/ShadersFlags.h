#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include <map>
#include "Game.h"
#include <d3dcompiler.h>

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 2049
#include "../GameFramework/magic_enum.hpp"

using namespace magic_enum::flags::bitwise_operators;



enum class PixelFlagsOpaque
{
	NONE = 1 << 0,
	INVERT_NORMAL = 1 << 1,
};


enum class VertexFlagsOpaque {
	NONE = 1 << 0,
	SKINNED_MESH = 1 << 1
};


enum class VertexFlagsLighting
{
	NONE = 1 << 0,
	SCREEN_QUAD = 1 << 1,
};


enum class PixelFlagsLighting : int
{
	AmbientLight = 1 << 0,
	DirectionalLight = 1 << 1,
	PointLight = 1 << 2,
	SpotLight = 1 << 3,
};


enum class PSToneMapFlags
{
	NONE = 1 << 0,
	CopyColor = 1 << 1,
	Simple = 1 << 2,
	WithLum = 1 << 3,
	WithLumEx = 1 << 4,
};


enum class VSToneMapFlags
{
	NONE = 1 << 0,
};



constexpr enum class PixelFlagsOpaque operator|(const enum class PixelFlagsOpaque& selfValue, const enum class PixelFlagsOpaque& inValue)
{
	return static_cast<enum class PixelFlagsOpaque>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class VertexFlagsOpaque operator|(const enum class VertexFlagsOpaque& selfValue, const enum class VertexFlagsOpaque& inValue)
{
	return static_cast<enum class VertexFlagsOpaque>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class VertexFlagsLighting operator|(const enum class VertexFlagsLighting& selfValue, const enum class VertexFlagsLighting& inValue)
{
	return static_cast<enum class VertexFlagsLighting>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class PixelFlagsLighting operator|(const enum class PixelFlagsLighting& selfValue, const enum class PixelFlagsLighting& inValue)
{
	return static_cast<enum class PixelFlagsLighting>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class PSToneMapFlags operator|(const enum class PSToneMapFlags& selfValue, const enum class PSToneMapFlags& inValue)
{
	return static_cast<enum class PSToneMapFlags>(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum class VSToneMapFlags operator|(const enum class VSToneMapFlags& selfValue, const enum class VSToneMapFlags& inValue)
{
	return static_cast<enum class VSToneMapFlags>(uint32_t(selfValue) | uint32_t(inValue));
}




template<typename T>
std::vector<D3D_SHADER_MACRO> GetMacroses(T flags)
{
	std::vector<D3D_SHADER_MACRO> macros;

	constexpr auto& entries = magic_enum::flags::enum_entries<T>();

	for (const std::pair<T, std::string_view>& p : entries)
	{
		if (static_cast<uint32_t>(flags & p.first) > 0) {
			D3D_SHADER_MACRO macro;
			macro.Name = p.second.data();
			macro.Definition = "1";

			macros.push_back(macro);
		}
	}

	macros.push_back(D3D_SHADER_MACRO{ nullptr, nullptr });
	return macros;
}


template<typename T>
void LoadVertexShaders(std::map<T, ID3D11VertexShader*>& _vertexShaders, const std::wstring& fileName, const std::vector<T>& flags, ID3DBlob*& _signature)
{
	_signature = nullptr;
	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;

	for (auto& flag : flags)
	{
		auto macros = GetMacroses(flag);
		auto res = D3DCompileFromFile(fileName.c_str(),
			&macros[0],
			nullptr/*Game::Instance->IncludeShaderHandler*/,
			"VSMain",
			"vs_5_0",
			D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&vertexBC,
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

		ID3D11VertexShader* vertexShader;
		Game::Instance->Device->CreateVertexShader(
			vertexBC->GetBufferPointer(),
			vertexBC->GetBufferSize(),
			nullptr, &vertexShader);

		if (_signature == nullptr) {
			_signature = vertexBC;
		}
		else {
			vertexBC->Release();
		}

		_vertexShaders.emplace(flag, vertexShader);
	}
}

template<typename T>
void LoadPixelShaders(std::map<T, ID3D11PixelShader*>& _pixelShaders, const std::wstring& fileName, const std::vector<T>& flags)
{
	ID3DBlob* pixelBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;

	for (auto& flag : flags) {
		auto macros = GetMacroses(flag);
		auto res = D3DCompileFromFile(fileName.c_str(),
			&macros[0],
			nullptr/*Game::Instance->IncludeShaderHandler*/,
			"PSMain",
			"ps_5_0",
			D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pixelBC,
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

		ID3D11PixelShader* pixelShader;
		Game::Instance->Device->CreatePixelShader(
			pixelBC->GetBufferPointer(),
			pixelBC->GetBufferSize(),
			nullptr, &pixelShader);

		_pixelShaders.emplace(flag, pixelShader);
	}
}



