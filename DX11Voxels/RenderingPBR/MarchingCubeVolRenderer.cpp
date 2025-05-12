#include "MarchingCubeVolRenderer.h"
#include <d3dcompiler.h>
#include "Noise/SimplexNoise.h"

#include "Camera.h"
#include "Game.h"
#include "InputDevice.h"


VolChunk::VolChunk(int inX, int inY, int inZ, unsigned int inSize) : X(inX), Y(inY), Z(inZ), Size(inSize)
{
	//GenerateTestData(Vector3::Zero);
	GenerateTestDataSphere({2.5f, 2.5f, 2.5f}, 1);

	unsigned int cubesCount = (Size - 1) * (Size - 1) * (Size - 1);
	UINT structSize = sizeof(Vector3) * 6;
	D3D11_BUFFER_DESC vertBufDesc{
		.ByteWidth = 5 * cubesCount * structSize, // 5 is max triangles in cube
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0,
		.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		.StructureByteStride = structSize
	};

	Game::Instance->Device->CreateBuffer(&vertBufDesc, nullptr, &appendBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{
		.Format = DXGI_FORMAT_UNKNOWN,
		.ViewDimension = D3D11_UAV_DIMENSION_BUFFER,
		.Buffer = D3D11_BUFFER_UAV {0, 5 * cubesCount, D3D11_BUFFER_UAV_FLAG_APPEND}
	};
	Game::Instance->Device->CreateUnorderedAccessView(appendBuffer, &uavDesc, &appendUAV);
	Game::Instance->Device->CreateShaderResourceView(appendBuffer, nullptr, &appendSRV);

	
	D3D11_BUFFER_DESC indirectBufDesc {
		.ByteWidth = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_UNORDERED_ACCESS,
		.CPUAccessFlags = 0,
		.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS,
		.StructureByteStride = 0
	};

	D3D11_UNORDERED_ACCESS_VIEW_DESC indirectUavDesc;
	indirectUavDesc.Format = DXGI_FORMAT_R32_UINT;
	indirectUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	indirectUavDesc.Buffer.FirstElement = 0;
	indirectUavDesc.Buffer.NumElements = 4;
	indirectUavDesc.Buffer.Flags = 0;

	UINT data[] = { 0,1,0,0 };
	D3D11_SUBRESOURCE_DATA indirectSource;
	indirectSource.SysMemSlicePitch = 0;
	indirectSource.SysMemPitch = 0;
	indirectSource.pSysMem = data;

	Game::Instance->Device->CreateBuffer(&indirectBufDesc, &indirectSource, &indirectBuffer);
	Game::Instance->Device->CreateUnorderedAccessView(indirectBuffer, &indirectUavDesc, &indirectUAV);


	D3D11_BUFFER_DESC counterBufDesc{
		.ByteWidth = 4,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_UNORDERED_ACCESS,
		.CPUAccessFlags = 0,
		.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		.StructureByteStride = 4
	};
	D3D11_UNORDERED_ACCESS_VIEW_DESC counterUavDesc;
	counterUavDesc.Format = DXGI_FORMAT_UNKNOWN;
	counterUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	counterUavDesc.Buffer.FirstElement = 0;
	counterUavDesc.Buffer.NumElements = 1;
	counterUavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	
	Game::Instance->Device->CreateBuffer(&counterBufDesc, nullptr, &counterBuffer);
	Game::Instance->Device->CreateUnorderedAccessView(counterBuffer, &counterUavDesc, &counterUAV);
}


void VolChunk::GenerateTestData(DirectX::SimpleMath::Vector3 offset)
{
	SafeRelease(volumeTex);
	SafeRelease(volumeSRV);
	SafeRelease(volumeUAV);


	float* data = new float[Size * Size * Size];
	for (unsigned int z = 0; z < Size; ++z) {
		for (unsigned int y = 0; y < Size; ++y) {
			for (unsigned int x = 0; x < Size; ++x) {
				data[x + y * Size + z * Size * Size] = SimplexNoise::noise(offset.x + (float)X + (float)x / (Size - 1), offset.y + (float)Y + (float)y / (Size - 1), offset.z + (float)Z + (float)z / (Size - 1));
			}
		}
	}

	D3D11_SUBRESOURCE_DATA dataSource;
	dataSource.SysMemSlicePitch = Size * Size * sizeof(float);
	dataSource.SysMemPitch = Size * sizeof(float);
	dataSource.pSysMem = data;


	D3D11_TEXTURE3D_DESC texDesc{
		Size, Size, Size,
		1,
		DXGI_FORMAT_R32_FLOAT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		0, 0
	};

	Game::Instance->Device->CreateTexture3D(&texDesc, &dataSource, &volumeTex);
	Game::Instance->Device->CreateShaderResourceView(volumeTex, nullptr, &volumeSRV);
	Game::Instance->Device->CreateUnorderedAccessView(volumeTex, nullptr, &volumeUAV);

	delete[] data;
}


float sdSphere(Vector3 p, Vector3 spherePos, float s)
{
	return (p-spherePos).Length() - s;
}

void VolChunk::GenerateTestDataSphere(DirectX::SimpleMath::Vector3 spherePos, float radius)
{
	SafeRelease(volumeTex);
	SafeRelease(volumeSRV);
	SafeRelease(volumeUAV);


	float* data = new float[Size * Size * Size];
	for (unsigned int z = 0; z < Size; ++z) {
		for (unsigned int y = 0; y < Size; ++y) {
			for (unsigned int x = 0; x < Size; ++x) {
				Vector3 pos = { (float)X + (float)x / (Size - 1), (float)Y + (float)y / (Size - 1), (float)Z + (float)z / (Size - 1) };
				data[x + y * Size + z * Size * Size] = sdSphere(pos, spherePos, radius);
			}
		}
	}

	D3D11_SUBRESOURCE_DATA dataSource;
	dataSource.SysMemSlicePitch = Size * Size * sizeof(float);
	dataSource.SysMemPitch = Size * sizeof(float);
	dataSource.pSysMem = data;


	D3D11_TEXTURE3D_DESC texDesc{
		Size, Size, Size,
		1,
		DXGI_FORMAT_R32_FLOAT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		0, 0
	};

	Game::Instance->Device->CreateTexture3D(&texDesc, &dataSource, &volumeTex);
	Game::Instance->Device->CreateShaderResourceView(volumeTex, nullptr, &volumeSRV);
	Game::Instance->Device->CreateUnorderedAccessView(volumeTex, nullptr, &volumeUAV);

	delete[] data;
}



MarchingCubeVolRenderer::MarchingCubeVolRenderer()
{
	LoadShaders();

	D3D11_SAMPLER_DESC sampDesc = {
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
		.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
		.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
		.MipLODBias = 0,
		.MaxAnisotropy = 16,
		.ComparisonFunc = D3D11_COMPARISON_ALWAYS,
		.BorderColor = {0,0,0,0},
		.MinLOD = 0,
		.MaxLOD = 0
	};


	Game::Instance->Device->CreateSamplerState(&sampDesc, &TrilinearClamp);
	

	D3D11_BUFFER_DESC constVolDesc{
		sizeof(PointVolConstParams),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_CONSTANT_BUFFER,
		0, 0, 0
	};

	Game::Instance->Device->CreateBuffer(&constVolDesc, nullptr, &constPointVolBuf);
	constData.ScaleParticleSizeThreshold = Vector4(20, 0.5f, 0.0f, 0.0f);

	int size = 32;

	constVolDesc.ByteWidth = sizeof(ComputeConstParams);
	Game::Instance->Device->CreateBuffer(&constVolDesc, nullptr, &constComputeBuf);
	computeConstData.SizeXYZScale = Vector4(static_cast<float>(size), static_cast<float>(size), static_cast<float>(size), 1.0f);


	int s = 5;
	for (int i = 0; i < s; ++i) {
		for (int j = 0; j < s; ++j) {
			for (int k = 0; k < s; ++k) {
				chunks.emplace_back(i, j, k, size);
			}
		}
	}
	
	//chunks.emplace_back(0, 2, 0, 20);
	//chunks.emplace_back(1, 2, 0, 20);

	CalculateMarchingCubes(0.0f);
}

void MarchingCubeVolRenderer::LoadShaders()
{
	ID3DBlob* shaderBC = nullptr;

	CompileShader(L"./Shaders/PointVolumeRendering.hlsl", "VSMain", "vs_5_0", shaderBC, false);
	Game::Instance->Device->CreateVertexShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &pointVolVS);
	SafeRelease(shaderBC);
	CompileShader(L"./Shaders/PointVolumeRendering.hlsl", "GSMain", "gs_5_0", shaderBC, false);
	Game::Instance->Device->CreateGeometryShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &pointVolGS);
	SafeRelease(shaderBC);
	CompileShader(L"./Shaders/PointVolumeRendering.hlsl", "PSMain", "ps_5_0", shaderBC, false);
	Game::Instance->Device->CreatePixelShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &pointVolPS);
	SafeRelease(shaderBC);


	CompileShader(L"./Shaders/NoiseFillVolume.hlsl", "FillVolume", "cs_5_0", shaderBC, false);
	Game::Instance->Device->CreateComputeShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &updateVolCS);
	SafeRelease(shaderBC);

	CompileShader(L"./Shaders/SDFFillVolume.hlsl", "SdfFillVolume", "cs_5_0", shaderBC, false);
	Game::Instance->Device->CreateComputeShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &sdfFillVolCS);
	SafeRelease(shaderBC);

	CompileShader(L"./Shaders/MarchingCubesCompute.hlsl", "GenerateMarchingCubes", "cs_5_0", shaderBC, false);
	Game::Instance->Device->CreateComputeShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &marchingCubeCS);
	SafeRelease(shaderBC);
	//CompileShader(L"./Shaders/MarchingCubesCompute.hlsl", "FillIndirect", "cs_5_0", shaderBC);
	//Game::Instance->Device->CreateComputeShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &fillIndirectCS);
	//SafeRelease(shaderBC);


	CompileShader(L"./Shaders/MarchingCubesRendering.hlsl", "VSMainBuf", "vs_5_0", shaderBC, false);
	Game::Instance->Device->CreateVertexShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &marchingRendBufVS);
	
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		D3D11_INPUT_ELEMENT_DESC { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	Game::Instance->Device->CreateInputLayout(layoutDesc, 2, shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), &marchingLayout);

	SafeRelease(shaderBC);

	CompileShader(L"./Shaders/MarchingCubesRendering.hlsl", "VSMain", "vs_5_0", shaderBC, false);
	Game::Instance->Device->CreateVertexShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &marchingRendVS);
	SafeRelease(shaderBC);

	CompileShader(L"./Shaders/MarchingCubesRendering.hlsl", "PSMain", "ps_5_0", shaderBC, false);
	Game::Instance->Device->CreatePixelShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &marchingRendPS);
	SafeRelease(shaderBC);

	CompileShader(L"./Shaders/MarchingCubesRendering.hlsl", "VSRenderToShadowMap", "vs_5_0", shaderBC, false);
	Game::Instance->Device->CreateVertexShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &VSRenderToShadowMap);
	SafeRelease(shaderBC);
	CompileShader(L"./Shaders/MarchingCubesRendering.hlsl", "GSRenderToShadowMap", "gs_5_0", shaderBC, false);
	Game::Instance->Device->CreateGeometryShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &GSRenderToShadowMap);
	SafeRelease(shaderBC);
}


void MarchingCubeVolRenderer::Update(float dt)
{
	auto ctx = Game::Instance->Context;

	if(prevOffset != Offset) {
		//for(auto& chunk : chunks)
		//	chunk.GenerateTestData(Offset);
		computeConstData.Offset = Vector4(Offset, 0);

		ctx->CSSetConstantBuffers(0, 1, &constComputeBuf);
		ctx->CSSetShader(updateVolCS, nullptr, 0);

		for (auto& chunk : chunks) {
			computeConstData.PositionIsoline = Vector4(static_cast<float>(chunk.X), static_cast<float>(chunk.Y), static_cast<float>(chunk.Z), 0);
			ctx->UpdateSubresource(constComputeBuf, 0, nullptr, &computeConstData, 0, 0);

			UINT counts[1] = { 0 };
			ctx->CSSetUnorderedAccessViews(0, 1, &chunk.volumeUAV, &counts[0]);

			const UINT groupCount = static_cast<UINT>(std::ceil(chunk.Size / 8.0f));
			ctx->Dispatch(groupCount, groupCount, groupCount);
		}
		prevOffset = Offset; 
	}

	chunksToDraw = chunks.size() - 1; // chunksToDraw < chunks.size() ? chunksToDraw : chunks.size() - 1;

	auto cam = Game::Instance->GameCamera;
	constData.World = Matrix::Identity;
	constData.View = cam->ViewMatrix;
	constData.Projection = cam->ProjMatrix;
}


void MarchingCubeVolRenderer::Render2()
{
	auto ctx = Game::Instance->Context;

	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	ctx->VSSetShader(pointVolVS, nullptr, 0);
	ctx->GSSetShader(pointVolGS, nullptr, 0);
	ctx->PSSetShader(pointVolPS, nullptr, 0);

	ctx->GSSetConstantBuffers(0, 1, &constPointVolBuf);

	for (int i = 0; i <= chunksToDraw; ++i) {
		const auto& chunk = chunks[i];
		ctx->GSSetShaderResources(0, 1, &chunk.volumeSRV);

		constData.PositionSize = Vector4((float)chunk.X, (float)chunk.Y, (float)chunk.Z, (float)chunk.Size);
		ctx->UpdateSubresource(constPointVolBuf, 0, nullptr, &constData, 0, 0);

		ctx->Draw(chunk.Size * chunk.Size * chunk.Size, 0);
	}

	ctx->VSSetShader(nullptr, nullptr, 0);
	ctx->GSSetShader(nullptr, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);
}


void MarchingCubeVolRenderer::RenderToShadowMap()
{
	auto ctx = Game::Instance->Context;

	Game::Instance->DebugAnnotation->BeginEvent(L"RenderVoxelsToShadowMap");

	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ctx->VSSetShader(VSRenderToShadowMap, nullptr, 0);
	ctx->GSSetShader(GSRenderToShadowMap, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);

	ctx->VSSetConstantBuffers(1, 1, &constPointVolBuf);

	for (int i = 0; i <= chunksToDraw; ++i) {
		const auto& chunk = chunks[i];

		constData.PositionSize = Vector4((float)chunk.X, (float)chunk.Y, (float)chunk.Z, (float)chunk.Size);
		ctx->UpdateSubresource(constPointVolBuf, 0, nullptr, &constData, 0, 0);

		ctx->VSSetShaderResources(0, 1, &chunk.appendSRV);

		ctx->DrawInstancedIndirect(chunk.indirectBuffer, 0);
	}

	ctx->VSSetShader(nullptr, nullptr, 0);
	ctx->GSSetShader(nullptr, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);

	Game::Instance->DebugAnnotation->EndEvent();
}


void MarchingCubeVolRenderer::AddSdfSphere(DirectX::SimpleMath::Vector3 spherePos, float radius, bool isSub)
{
	auto ctx = Game::Instance->Context;
	
	computeConstData.Offset = Vector4(spherePos / constData.ScaleParticleSizeThreshold.x, radius);

	ctx->CSSetConstantBuffers(0, 1, &constComputeBuf);
	ctx->CSSetShader(sdfFillVolCS, nullptr, 0);

	for (auto& chunk : chunks) {
		computeConstData.PositionIsoline = Vector4(static_cast<float>(chunk.X), static_cast<float>(chunk.Y), static_cast<float>(chunk.Z), isSub);
		ctx->UpdateSubresource(constComputeBuf, 0, nullptr, &computeConstData, 0, 0);

		UINT counts[1] = { static_cast<UINT >(-1)};
		ctx->CSSetUnorderedAccessViews(0, 1, &chunk.volumeUAV, &counts[0]);

		const UINT groupCount = static_cast<UINT>(std::ceil(chunk.Size / 8.0f));
		ctx->Dispatch(groupCount, groupCount, groupCount);
	}

	CalculateMarchingCubes(0);
}


void MarchingCubeVolRenderer::Render()
{
	auto ctx = Game::Instance->Context;

	Game::Instance->DebugAnnotation->BeginEvent(L"RenderVoxels");

	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ctx->VSSetShader(marchingRendVS, nullptr, 0);
	ctx->PSSetShader(marchingRendPS, nullptr, 0);

	ctx->VSSetConstantBuffers(0, 1, &constPointVolBuf);


	for (int i = 0; i <= chunksToDraw; ++i) {
		const auto& chunk = chunks[i];

		constData.PositionSize = Vector4((float)chunk.X, (float)chunk.Y, (float)chunk.Z, (float)chunk.Size);
		ctx->UpdateSubresource(constPointVolBuf, 0, nullptr, &constData, 0, 0);

		ctx->VSSetShaderResources(0, 1, &chunk.appendSRV);
		ctx->DrawInstancedIndirect(chunk.indirectBuffer, 0);
	}

	ctx->VSSetShader(nullptr, nullptr, 0);
	ctx->GSSetShader(nullptr, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);

	Game::Instance->DebugAnnotation->EndEvent();
}


void MarchingCubeVolRenderer::CalculateMarchingCubes(float isolineVal)
{
	auto ctx = Game::Instance->Context;

	computeConstData.Offset = Vector4(Offset, 0);

	ctx->CSSetConstantBuffers(0, 1, &constComputeBuf);
	ctx->CSSetShader(marchingCubeCS, nullptr, 0);
	ctx->CSSetSamplers(0, 1, &TrilinearClamp);


	for (auto& chunk : chunks) {
		computeConstData.PositionIsoline = Vector4(static_cast<float>(chunk.X), static_cast<float>(chunk.Y), static_cast<float>(chunk.Z), isolineVal);
		ctx->UpdateSubresource(constComputeBuf, 0, nullptr, &computeConstData, 0, 0);

		int indBufData[] = { 0, 1, 0, 0 };
		ctx->UpdateSubresource(chunk.indirectBuffer, 0, nullptr, indBufData, 0, 0);

		UINT counts[1] = { 0 };
		ctx->CSSetUnorderedAccessViews(0, 1, &chunk.appendUAV, counts);
		ctx->CSSetUnorderedAccessViews(1, 1, &chunk.indirectUAV, counts);
		ctx->CSSetShaderResources(0, 1, &chunk.volumeSRV);

		const UINT groupCount = static_cast<UINT>(std::ceil(chunk.Size / 8.0f));
		ctx->Dispatch(groupCount, groupCount, groupCount);

		ID3D11UnorderedAccessView* nullViews[] = { nullptr, nullptr };
		UINT nullCounts[] = { 0, 0 };
		ctx->CSSetUnorderedAccessViews(0, 2, nullViews, nullCounts);
	}

	//ctx->CSSetShader(fillIndirectCS, nullptr, 0);
	//for (auto& chunk : chunks) {
	//	ctx->CopyStructureCount(chunk.indirectBuffer, 0, chunk.counterUAV);		
	//}
}
