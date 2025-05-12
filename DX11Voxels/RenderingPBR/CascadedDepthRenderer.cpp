#include "CascadedDepthRenderer.h"
#include <vector>
#include <d3d11.h>

#include "Camera.h"
#include "DebugRenderSystem.h"
#include "Game.h"
#include "InputDevice.h"
#include "../ZMathLib/SimpleMath.h"

using namespace DirectX::SimpleMath;


constexpr int CASCADE_COUNT = 4;

#pragma pack(push, 4)
struct CascadeData
{
	Matrix viewProjMats[CASCADE_COUNT];
	float distances[CASCADE_COUNT];
};
#pragma pack(pop)


CascadedDepthRenderer::CascadedDepthRenderer(int inWidth, int inHeight) : width(inWidth), height(inHeight)
{
	auto context = Game::Instance->Context;
	auto device = Game::Instance->Device;

	// init shaders
	ID3DBlob* _signatureOpaq = nullptr;
	LoadVertexShaders(depthVSs, L"./Shaders/CascadedDepthRenderer.hlsl", vsFlagsDepth, _signatureOpaq);
	SafeRelease(_signatureOpaq);

	ID3DBlob* shaderBC;
	ID3DBlob* errorVertexCode;
	auto res = D3DCompileFromFile(L"./Shaders/CascadedDepthRenderer.hlsl",
		nullptr /*macros*/,
		nullptr /*include*/,
		"GSMain",
		"gs_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shaderBC,
		&errorVertexCode);
	
	if (FAILED(res)) {
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		} else {
			std::cout << "Missing Shader File\n";
		}
	}

	device->CreateGeometryShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &depthGS);

	SafeRelease(errorVertexCode);
	SafeRelease(shaderBC);


	// init buffers
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;
	bufDesc.ByteWidth = sizeof(CascadeData);

	device->CreateBuffer(&bufDesc, nullptr, &cascadeBuf);

	bufDesc.ByteWidth = sizeof(Matrix);
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.CPUAccessFlags = 0;
	device->CreateBuffer(&bufDesc, nullptr, &constBuf);
	

	// init textures
	D3D11_TEXTURE2D_DESC depthDescription = {};
	depthDescription.Width = width;
	depthDescription.Height = height;
	depthDescription.ArraySize = CASCADE_COUNT;
	depthDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthDescription.CPUAccessFlags = 0;
	depthDescription.Format = DXGI_FORMAT_R32_TYPELESS,
	depthDescription.MipLevels = 1,
	depthDescription.MiscFlags = 0,
	depthDescription.SampleDesc.Count = 1;
	depthDescription.SampleDesc.Quality = 0;
	depthDescription.Usage = D3D11_USAGE_DEFAULT,

	device->CreateTexture2D(&depthDescription, nullptr, &shadowTexArr);


	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc = {};
	dViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dViewDesc.Texture2DArray = { 0, 0, 4 };

	device->CreateDepthStencilView(shadowTexArr, &dViewDesc, &depthDSV);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray = { 0, 1, 0, 4 };

	device->CreateShaderResourceView(shadowTexArr, &srvDesc, &depthSRV);


	viewport = new D3D11_VIEWPORT{};
	viewport->Width = static_cast<float>(width);
	viewport->Height = static_cast<float>(height);
	viewport->TopLeftX = 0;
	viewport->TopLeftY = 0;
	viewport->MinDepth = 0;
	viewport->MaxDepth = 1.0f;
}

void CascadedDepthRenderer::Update(Camera* inCamera, Vector3 inLightDirection)
{
	if (!inCamera) return;
	auto cam = inCamera;

	CascadeData data{};

	auto dist		= cam->FarPlaneDistance - cam->NearPlaneDistance;
	auto aspect		= cam->AspectRatio;
	auto fov		= cam->Fov;

	float parts[] = {1, 2, 3, 4};
	float sum = 10;
	float acc = cam->NearPlaneDistance;
	for (int i = 0; i < CASCADE_COUNT; ++i) {
		acc += parts[i] / sum * dist;
		data.distances[i] = acc;
	}

	for(int i = 0; i < CASCADE_COUNT; ++i)
	{
		float nearPlane = i == 0 ? cam->NearPlaneDistance : data.distances[i-1];
		float farPlane = data.distances[i];

		auto projMat = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
			fov,
			aspect,
			nearPlane,
			farPlane);

		auto corners = MathHelper::GetFrustumCornersWorldSpace(cam->ViewMatrix, projMat);


		/////////////////// CALCULATE VIEW MATRIX ////////////////////////
		Vector3 center = Vector3::Zero;
		for (const auto& v : corners) {
			center += v.ToVec3();								///// TODO: MERGE THIS WITH FINDING MIN MAX
		}
		center /= corners.size();

		const auto lightView = Matrix::CreateLookAt(
			center,
			center - inLightDirection,
			Vector3::Up											// TODO: check case when sun direction is {0,1,0}
		);
		/////////////////////////////////////////////////////////////////
		///
		///////////////// CALCULATE PROJ MATRIX /////////////////////////
		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::lowest();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::lowest();
		for (const auto& v : corners) {
			const auto trf = Vector4::Transform(v, lightView);

			minX = std::min(minX, trf.x);
			maxX = std::max(maxX, trf.x);
			minY = std::min(minY, trf.y);
			maxY = std::max(maxY, trf.y);
			minZ = std::min(minZ, trf.z);
			maxZ = std::max(maxZ, trf.z);
		}

		constexpr float zMult = 10.0f;
		minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
		maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;
	
		auto lightProjection = Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ); // TODO: check case when sun direction is {0,1,0}

		//Game::Instance->DebugRender->DrawFrustrum(lightView, lightProjection);

		data.viewProjMats[i] = lightView * lightProjection;
	}

	//Game::Instance->Context->UpdateSubresource(cascadeBuf, 0, nullptr, &data, 0, 0);
	D3D11_MAPPED_SUBRESOURCE mapped{};
	auto res = Game::Instance->Context->Map(cascadeBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	memcpy(mapped.pData, &data, sizeof(CascadeData));

	Game::Instance->Context->Unmap(cascadeBuf, 0);
}
