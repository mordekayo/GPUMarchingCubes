#include <iostream>
#include <vector>

#include "../GameFramework/Game.h"
#include "RenderingSystemPBR.h"

#include "VertexSkin.h"
#include "../GameFramework/VertexPositionNormalBinormalTangentColorTex.h"
#include "../GameFramework/VertexPositionColor.h"


void RenderingSystemPBR::ReloadShadersOpaque()
{
	std::map<VertexFlagsOpaque, ID3D11VertexShader*>	_vertexShadersOpaq;
	std::map<PixelFlagsOpaque, ID3D11PixelShader*>		_pixelShadersOpaq;

	std::map<VertexFlagsLighting, ID3D11VertexShader*>	_vertexShadersLigh;
	std::map<PixelFlagsLighting, ID3D11PixelShader*>	_pixelShadersLigh;

	ID3DBlob* _signatureOpaq = nullptr;
	ID3DBlob* _signatureLigh = nullptr;

	LoadVertexShaders(_vertexShadersOpaq, L"./Shaders/PBRFillGBuffer.hlsl", vsFlagsOpaque, _signatureOpaq);
	LoadPixelShaders(_pixelShadersOpaq, L"./Shaders/PBRFillGBuffer.hlsl", psFlagsOpaque);
	
	LoadVertexShaders(_vertexShadersLigh, L"./Shaders/PBRLighting.hlsl", vsFlagsLighting, _signatureLigh);
	LoadPixelShaders(_pixelShadersLigh, L"./Shaders/PBRLighting.hlsl", psFlagsLighting);


	SafeRelease(layoutOpaque);
	layoutOpaque = VertexPositionNormalBinormalTangentColorTex::GetLayout(_signatureOpaq);
	SafeRelease(layoutLighting);
	layoutLighting = VertexPositionColor::GetLayout(_signatureLigh);


	std::vector<D3D_SHADER_MACRO> macros;
	macros.push_back(D3D_SHADER_MACRO{ "SKINNED_MESH", "1" });
	macros.push_back(D3D_SHADER_MACRO{ nullptr, nullptr });

	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;
	auto res = D3DCompileFromFile(L"./Shaders/PBRFillGBuffer.hlsl",
		&macros[0],
		nullptr/*Game::Instance->IncludeShaderHandler*/,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexBC,
		&errorVertexCode
	);

	layoutOpaqueAnimated = VertexSkin::GetLayout(vertexBC);

	for(auto& val : OpaqueVSs) {
		SafeRelease(val.second);
	}
	for(auto& val : OpaquePSs) {
		SafeRelease(val.second);
	}
	for(auto& val : LightingVSs) {
		SafeRelease(val.second);
	}
	for(auto& val : LightingPSs) {
		SafeRelease(val.second);
	}


	OpaqueVSs = _vertexShadersOpaq;
	OpaquePSs = _pixelShadersOpaq;

	LightingVSs = _vertexShadersLigh;
	LightingPSs = _pixelShadersLigh;


	_signatureOpaq->Release();
	_signatureLigh->Release();
}


void RenderingSystemPBR::ReloadShadersToneMap()
{
	std::map<VSToneMapFlags, ID3D11VertexShader*>	vertexShadersTone;
	std::map<PSToneMapFlags, ID3D11PixelShader*>	pixelShadersTone;

	ID3DBlob* _signatureOpaq = nullptr;

	LoadVertexShaders(vertexShadersTone, L"./Shaders/PBRToneMapping.hlsl", vsFlagsTone, _signatureOpaq);
	LoadPixelShaders(pixelShadersTone, L"./Shaders/PBRToneMapping.hlsl", psFlagsTone);

	for (auto& val : ToneVSs) {
		SafeRelease(val.second);
	}
	for (auto& val : TonePSs) {
		SafeRelease(val.second);
	}

	ToneVSs = vertexShadersTone;
	TonePSs = pixelShadersTone;

	SafeRelease(_signatureOpaq);
}

