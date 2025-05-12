#pragma once
#include "../GameFramework/Game.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;



struct GBuffer
{
	ID3D11Texture2D* DiffuseTex			= nullptr;
	ID3D11Texture2D* NormalTex			= nullptr;
	ID3D11Texture2D* MetRougAoIdTex		= nullptr;
	ID3D11Texture2D* EmissiveTex		= nullptr;
	ID3D11Texture2D* WorldPositionTex	= nullptr;

	ID3D11Texture2D* AccumulationBuf	= nullptr;
	ID3D11Texture2D* BloomTex			= nullptr;

	
public:
	ID3D11ShaderResourceView* DiffuseSRV		= nullptr;
	ID3D11ShaderResourceView* NormalSRV			= nullptr;
	ID3D11ShaderResourceView* MetRougAoIdSRV		= nullptr;
	ID3D11ShaderResourceView* EmissiveSRV		= nullptr;
	ID3D11ShaderResourceView* WorldPositionSRV	= nullptr;

	ID3D11RenderTargetView* DiffuseRTV			= nullptr;
	ID3D11RenderTargetView* NormalRTV			= nullptr;
	ID3D11RenderTargetView* MetRougAoIdRTV		= nullptr;
	ID3D11RenderTargetView* EmissiveRTV			= nullptr;
	ID3D11RenderTargetView* WorldPositionRTV	= nullptr;
		

	ID3D11ShaderResourceView* AccumulationSRV	= nullptr;
	ID3D11ShaderResourceView* BloomSRV			= nullptr;

	ID3D11RenderTargetView* AccumulationRTV = nullptr;
	ID3D11RenderTargetView* BloomRTV		= nullptr;


public:
	GBuffer(int width, int height);

	void Resize(int width, int height);


	void Dispose();
};


#pragma pack(push, 4)
struct GSampleData
{
	Vector3	WorldPosition;
	float	Distance;
	Vector3	WorldNormal;
	int		Id;
};
#pragma pack(pop)

