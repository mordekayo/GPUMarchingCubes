#pragma once
#include <d3d11.h>
#include "../ZMathLib/SimpleMath.h"


struct VolChunk
{
	int X, Y, Z;
	unsigned int Size;
	ID3D11Texture3D*			volumeTex = nullptr;
	ID3D11ShaderResourceView*	volumeSRV = nullptr;
	ID3D11UnorderedAccessView*	volumeUAV = nullptr;

	ID3D11Buffer* appendBuffer;
	ID3D11UnorderedAccessView* appendUAV;
	ID3D11ShaderResourceView* appendSRV;

	ID3D11Buffer* indirectBuffer;
	ID3D11UnorderedAccessView* indirectUAV;

	ID3D11Buffer* counterBuffer;
	ID3D11UnorderedAccessView* counterUAV;

	VolChunk(int inX, int inY, int inZ, unsigned int inSize);

	void GenerateTestData(DirectX::SimpleMath::Vector3 offset);
	void GenerateTestDataSphere(DirectX::SimpleMath::Vector3 spherePos, float radius);
};


struct MarchingCubeVolRenderer
{
	ID3D11VertexShader*		pointVolVS = nullptr;
	ID3D11GeometryShader*	pointVolGS = nullptr;
	ID3D11PixelShader*		pointVolPS = nullptr;

	ID3D11ComputeShader* updateVolCS	= nullptr;
	ID3D11ComputeShader* sdfFillVolCS	= nullptr;
	ID3D11ComputeShader* marchingCubeCS	= nullptr;
	ID3D11ComputeShader* fillIndirectCS = nullptr;

	ID3D11VertexShader*	marchingRendVS = nullptr;
	ID3D11VertexShader* marchingRendBufVS = nullptr;
	ID3D11PixelShader*	marchingRendPS = nullptr;
	ID3D11InputLayout*	marchingLayout = nullptr;

	ID3D11VertexShader*		VSRenderToShadowMap = nullptr;
	ID3D11GeometryShader*	GSRenderToShadowMap = nullptr;

	ID3D11SamplerState* TrilinearClamp;


	struct PointVolConstParams
	{
		DirectX::SimpleMath::Matrix World;
		DirectX::SimpleMath::Matrix View;
		DirectX::SimpleMath::Matrix Projection;
		DirectX::SimpleMath::Vector4 PositionSize;
		DirectX::SimpleMath::Vector4 ScaleParticleSizeThreshold;
	};

	struct ComputeConstParams
	{
		DirectX::SimpleMath::Vector4 SizeXYZScale;
		DirectX::SimpleMath::Vector4 PositionIsoline;
		DirectX::SimpleMath::Vector4 Offset;
	};

	ID3D11Buffer* constPointVolBuf	= nullptr;
	ID3D11Buffer* constComputeBuf	= nullptr;

public:
	std::vector<VolChunk> chunks{};

	PointVolConstParams	constData;
	ComputeConstParams	computeConstData;
	DirectX::SimpleMath::Vector3 Offset;
	DirectX::SimpleMath::Vector3 prevOffset;
	int chunksToDraw = 4;

public:
	MarchingCubeVolRenderer();

	void LoadShaders();
	void Update(float dt);
	void Render();
	void Render2();
	void RenderToShadowMap();

	void AddSdfSphere(DirectX::SimpleMath::Vector3 spherePos, float radius, bool isSub = false);

	void CalculateMarchingCubes(float isolineVal);
};

