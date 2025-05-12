#include "RenderingSystemPBR.h"

#include "CascadedDepthRenderer.h"
#include "DebugRenderSystem.h"
#include "InputDevice.h"
#include "SkyRenderer.h"
#include "../GameFramework/Game.h"
#include "../GameFramework/Camera.h"
#include "../GameFramework/CubeMapHelper.h"
#include "../GameFramework/StaticMesh.h"
#include "../GameFramework/SkyBoxComp.h"
#include "ZPostProcess.h"



RenderingSystemPBR::RenderingSystemPBR(Game* game) : game(game), gBuffer(game->Display->ClientWidth, game->Display->ClientHeight)
{
	//volRenderer = new MarchingCubeVolRenderer();
	voxelTree = new VoxelTree(*game->GameCamera);


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MaxLOD = static_cast<float>(INT_MAX);
	//samplerDesc.MaxAnisotropy = 1.0f;

	game->Device->CreateSamplerState(&samplerDesc, &samplerWrap);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	game->Device->CreateSamplerState(&samplerDesc, &samplerClamp);


	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;

	game->Device->CreateSamplerState(&samplerDesc, &shadowSampler);
	
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	game->Device->CreateRasterizerState(&rastDesc, &rastCullBack);
	
	rastDesc.CullMode = D3D11_CULL_FRONT;
	game->Device->CreateRasterizerState(&rastDesc, &rastCullFront);

	rastDesc.CullMode = D3D11_CULL_NONE;
	game->Device->CreateRasterizerState(&rastDesc, &rastCullNone);

	rastDesc.FillMode = D3D11_FILL_WIREFRAME;
	game->Device->CreateRasterizerState(&rastDesc, &rastWireCullNone);

	auto blendStateDesc = D3D11_BLEND_DESC{ false, false };
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].BlendOp		= D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].BlendOpAlpha	= D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlend		= D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlend	= D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ZERO;

	game->Device->CreateBlendState(&blendStateDesc, &blendStateOpaque);

	blendStateDesc.RenderTarget[0].SrcBlend		= D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend	= D3D11_BLEND_ONE;
	game->Device->CreateBlendState(&blendStateDesc, &blendStateLight);
	

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;
	bufDesc.ByteWidth = sizeof(ConstantStruct);

	game->Device->CreateBuffer(&bufDesc, nullptr, &constantBuffer);

	bufDesc.ByteWidth = sizeof(MaterialStruct);
	game->Device->CreateBuffer(&bufDesc, nullptr, &materialBuffer);


	bufDesc.ByteWidth = sizeof(float) * 4;
	game->Device->CreateBuffer(&bufDesc, nullptr, &constantToneBuffer);
	
	bufDesc.ByteWidth = sizeof(LightData);
	game->Device->CreateBuffer(&bufDesc, nullptr, &lightsBuffer);

	bufDesc.ByteWidth = sizeof(FogData);
	game->Device->CreateBuffer(&bufDesc, nullptr, &fogBuffer);


	ReloadShadersOpaque();
	ReloadShadersToneMap();


	game->ScreenResized.AddRaw(this, &RenderingSystemPBR::Game_ScreenResize);
	
	auto depthDesc = D3D11_DEPTH_STENCIL_DESC{};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.StencilEnable = false;
	depthDesc.StencilReadMask = 0x00;
	depthDesc.StencilWriteMask = 0x00;

	game->Device->CreateDepthStencilState(&depthDesc, &depthStateOpaque);

	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	game->Device->CreateDepthStencilState(&depthDesc, &dsLightingLess);

	depthDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	game->Device->CreateDepthStencilState(&depthDesc, &dsLightingGreater);

	depthDesc.DepthEnable = false;
	game->Device->CreateDepthStencilState(&depthDesc, &dsNoDepth);


	cubeHelper = new CubeMapHelper();
	cubeHelper->Init(game);

	skyMap = new ZTexViews();
	preMap = new ZTexViews();
	convMap = new ZTexViews();
	intMap = new ZTexViews();

	//cubeHelper->ConvertHDR(L"./Content/14-Hamarikyu_Bridge_B_3k.hdr", *skyMap, true);
	cubeHelper->LoadFromDDS(L"./Content/14-Hamarikyu_Bridge_B_3k.dds", *skyMap);
	cubeHelper->LoadFromDDS(L"./Content/HdrMaps/japanHdrDiffuseHDR.dds", *convMap);

	cubeHelper->PreFilterMipMaps(*skyMap, *preMap);
	cubeHelper->GenerateIntegratedBRDFMap(*intMap);
	//cubeHelper->SaveToDDS(intMap->Tex, L"./Content/test.dds");

	//cubeHelper->LoadFromDDS()

	skyBox = new SkyBoxGameComponent(game, game->GameCamera);
	skyBox->Initialize();

	skyBox->cubeMap = skyMap->Tex;
	skyBox->SRV = skyMap->Srv;

	skyRend = new SkyRenderer(game);
	skyRend->BackBufferResized(game->Display->ClientWidth, game->Display->ClientHeight, 1);
	skyRend->Init();
	skyRend->Update(1);

	depthRenderer = new CascadedDepthRenderer(4096, 4096);


	D3D11_BUFFER_DESC sampleBufDesc = {};
	sampleBufDesc.Usage = D3D11_USAGE_DEFAULT;
	sampleBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sampleBufDesc.CPUAccessFlags = 0;
	sampleBufDesc.MiscFlags = 0;
	sampleBufDesc.StructureByteStride = 0;
	sampleBufDesc.ByteWidth = sizeof(Vector4);
	game->Device->CreateBuffer(&sampleBufDesc, nullptr, &sampleCB);


	sampleBufDesc.Usage = D3D11_USAGE_DEFAULT;
	sampleBufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	sampleBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	sampleBufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sampleBufDesc.StructureByteStride = sizeof(GSampleData);
	sampleBufDesc.ByteWidth = sizeof(GSampleData);
	game->Device->CreateBuffer(&sampleBufDesc, nullptr, &sampleDataBuffer);


	D3D11_UNORDERED_ACCESS_VIEW_DESC sampleUavDesc;
	sampleUavDesc.Format = DXGI_FORMAT_UNKNOWN;
	sampleUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	sampleUavDesc.Buffer.FirstElement	= 0;
	sampleUavDesc.Buffer.NumElements	= 1;
	sampleUavDesc.Buffer.Flags = 0;
	game->Device->CreateUnorderedAccessView(sampleDataBuffer, &sampleUavDesc, &sampleUAV);

	ID3DBlob* shaderBC = nullptr;
	CompileShader(L"./Shaders/GBufferSampling.hlsl", "SampleGBuffer", "cs_5_0", shaderBC, false);
	Game::Instance->Device->CreateComputeShader(shaderBC->GetBufferPointer(), shaderBC->GetBufferSize(), nullptr, &sampleCS);
	SafeRelease(shaderBC);
}


void RenderingSystemPBR::Game_ScreenResize(const Vector2& newSize)
{
	gBuffer.Resize(static_cast<int>(newSize.x), static_cast<int>(newSize.y));
}


void RenderingSystemPBR::ShadowMapPass(float deltaTime)
{
	if (!DirectionalLights.size()) return;

	auto context = game->Context;

	context->RSSetState(depthBack);
	context->OMSetBlendState(blendStateOpaque, nullptr, 0xffffffff);
	context->OMSetDepthStencilState(depthStateOpaque, 0);

	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->GSSetShader(depthRenderer->depthGS, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

	context->VSSetConstantBuffers(0, 1, &depthRenderer->constBuf);
	context->GSSetConstantBuffers(0, 1, &depthRenderer->cascadeBuf);

	context->RSSetViewports(1, depthRenderer->viewport);

	context->ClearDepthStencilView(depthRenderer->depthDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);


	ID3D11RenderTargetView* views[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	context->OMSetRenderTargets(8, views, depthRenderer->depthDSV);

	game->DebugAnnotation->BeginEvent(L"Shadows Pass");
	for (auto& mesh : Meshes)
	{
		if (mesh->IsSkinned) {
			context->IASetInputLayout(layoutOpaqueAnimated);
			context->VSSetShader(depthRenderer->depthVSs[VertexFlagsOpaque::SKINNED_MESH], nullptr, 0);

			context->VSSetShaderResources(0, 1, &mesh->AnimFrame);

		}
		else {
			context->IASetInputLayout(layoutOpaque);
			context->VSSetShader(depthRenderer->depthVSs[VertexFlagsOpaque::NONE], nullptr, 0);
		}

		auto World = mesh->Transform;
		game->Context->UpdateSubresource(depthRenderer->constBuf, 0, nullptr, &World, 0, 0);

		unsigned int offset = 0;
		context->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		for (auto& subset : mesh->Subsets) {
			context->DrawIndexed(subset.PrimitiveCount * 3, subset.StartPrimitive * 3, 0);
		}

	}
	//context->RSSetState(rastCullNone);
	//volRenderer->RenderToShadowMap();
	game->DebugAnnotation->EndEvent();

	context->GSSetShader(nullptr, nullptr, 0);
	context->OMSetRenderTargets(8, views, nullptr);

	//game->DebugRender->DrawTextureOnScreen(depthRenderer->depthSRV, 10, 10, 256, 256);
}

void RenderingSystemPBR::ResetShadowRasterizerState(int DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias)
{
	SafeRelease(depthBack);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthBias = DepthBias;
	rastDesc.DepthBiasClamp = DepthBiasClamp;
	rastDesc.SlopeScaledDepthBias = SlopeScaledDepthBias;

	game->Device->CreateRasterizerState(&rastDesc, &depthBack);
}


void RenderingSystemPBR::Update(float deltaTime)
{
	constData.View = game->GameCamera->ViewMatrix;
	constData.Proj = game->GameCamera->ProjMatrix;
	constData.PerspectiveMatVals = Vector4(1.0f / constData.Proj.m[0][0], 1.0f / constData.Proj.m[0][0], constData.Proj.m[3][2], -constData.Proj.m[2][2]);

	const auto camPos = game->GameCamera->GetPosition();
	constData.ViewerPos = Vector4(camPos.x, camPos.y, camPos.z, 1.0f);

	fogData.FogSunDir = skyRend->Direction;
	game->Context->UpdateSubresource(fogBuffer, 0, nullptr, &fogData, 0, 0);

	if(game->InputDevice->IsKeyDown(Keys::R))
	{
		cubeHelper->ReloadShaders();
		cubeHelper->PreFilterMipMaps(*skyMap, *preMap);
		skyBox->cubeMap = preMap->Tex;
		skyBox->SRV = preMap->Srv;
	}

	if (DirectionalLights.size()) {
		depthRenderer->Update(game->GameCamera, DirectionalLights[0]->PosDir);
	}
	//game->DebugRender->DrawTextureOnScreen(gBuffer.NormalSRV, 10, 10, 128, 128);

	if (!game->InputDevice->IsKeyDown(Keys::RightShift))
		for (auto& postProc : PostProcesses) {
			postProc->Update(deltaTime);
		}
	
	//skyBox->Update(deltaTime);
	skyRend->Update(deltaTime);

	//volRenderer->Update(deltaTime);
	voxelTree->Update();
}


void RenderingSystemPBR::Draw(float deltaTime)
{
	game->Context->ClearState();

	Game::Instance->GpuProfiler->AddTimestamp("ShadowMapPass begin");
	ShadowMapPass(deltaTime);
	Game::Instance->GpuProfiler->AddTimestamp("ShadowMapPass end");

	Game::Instance->GpuProfiler->AddTimestamp("OpaquePass begin");
	OpaquePass(deltaTime);
	Game::Instance->GpuProfiler->AddTimestamp("OpaquePass end");

	game->Context->RSSetState(rastCullBack);

	if (game->InputDevice->IsKeyDown(Keys::F4))
		game->Context->RSSetState(rastWireCullNone);
	//volRenderer->Render();
	voxelTree->Draw();

	LightingPass(deltaTime);


	if(!game->InputDevice->IsKeyDown(Keys::RightShift))
		for (auto& postProc : PostProcesses) {
			postProc->Draw(gBuffer, deltaTime);
		}

	ToneMapPass(deltaTime);


	//game->Context->RSSetState(rastCullBack);
	//game->Context->OMSetDepthStencilState(depthStateOpaque, 0);
	//volRenderer->Render2();


	if(currentRequest) {
		if(currentRequest->sampled == true) {

			GSampleData data{};
			D3D11_MAPPED_SUBRESOURCE resource;
			game->Context->Map(sampleDataBuffer, 0, D3D11_MAP_READ, 0, &resource);
			memcpy(&data, resource.pData, sizeof(GSampleData));
			game->Context->Unmap(sampleDataBuffer, 0);

			currentRequest->callback(data);
			delete currentRequest;
			currentRequest = nullptr;
		} else {
			const Vector4 screenPos{ static_cast<float>(currentRequest->x), static_cast<float>(currentRequest->y), 0, 0 };
			game->Context->UpdateSubresource(sampleCB, 0, nullptr, &screenPos, 0, 0);

			game->Context->CSSetShader(sampleCS, nullptr, 0);
			game->Context->CSSetConstantBuffers(0, 1, &sampleCB);
			game->Context->CSSetUnorderedAccessViews(0, 1, &sampleUAV, nullptr);
			
			ID3D11ShaderResourceView* resources[] = { gBuffer.DiffuseSRV, gBuffer.NormalSRV, gBuffer.MetRougAoIdSRV, gBuffer.EmissiveSRV, gBuffer.WorldPositionSRV };
			game->Context->CSSetShaderResources(0, 5, resources);

			game->Context->Dispatch(1, 1, 1);

			currentRequest->sampled = true;
		}
	}


	//var fromRes = gBuffer.AccumulationSRV.Resource;
	//var destRes = game.RenderView.Resource;
	//game.Context.CopyResource(fromRes, destRes);
	//
	//fromRes.Dispose();
	//destRes.Dispose();
	//
	//game.Context.ClearState();
	//game.RestoreTargets();
}


void RenderingSystemPBR::RequestScreenSample(int x, int y, const std::function<void(GSampleData&)>& callback)
{
	if (currentRequest)
		return;

	currentRequest = new ScreenSampleRequest{ x, y, false, callback };
}


void RenderingSystemPBR::RegisterMesh(std::shared_ptr <StaticMesh> mesh)
{
	Meshes.emplace_back(mesh);
}


void RenderingSystemPBR::RegisterMeshes(const std::vector<std::shared_ptr<StaticMesh>>& meshes)
{
	Meshes.insert(Meshes.end(), meshes.begin(), meshes.end());
}


void RenderingSystemPBR::UnregisterMesh(std::shared_ptr<StaticMesh> mesh)
{
	//std::ranges::remove(Meshes, mesh);
	std::erase(Meshes, mesh);
}


void RenderingSystemPBR::RegisterLight(LightBase* lBase)
{
	Lights.emplace_back(lBase);

	auto dirLight = dynamic_cast<DirectionalLight*>(lBase);
	if (dirLight) DirectionalLights.emplace_back(dirLight);
}


void RenderingSystemPBR::UnregisterLight(LightBase* lBase)
{
	std::erase(Lights, lBase);

	auto dirLight = dynamic_cast<DirectionalLight*>(lBase);
	if (dirLight) std::erase(DirectionalLights, dirLight);
}

void RenderingSystemPBR::RegisterPostProcess(std::shared_ptr<ZPostProcess> postProcess)
{
	PostProcesses.push_back(postProcess);
}

void RenderingSystemPBR::UnregisterPostProcess(std::shared_ptr<ZPostProcess> postProcess)
{
	std::erase(PostProcesses, postProcess);
}


