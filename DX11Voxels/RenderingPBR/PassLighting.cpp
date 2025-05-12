#include "RenderingSystemPBR.h"
#include "SkyRenderer.h"
#include "../GameFramework/Game.h"
#include "../GameFramework/Camera.h"
#include "../GameFramework/SkyBoxComp.h"
#include "../GameFramework/CubeMapHelper.h"
#include "CascadedDepthRenderer.h"


void RenderingSystemPBR::LightingPass(float deltaTime)
{
	auto context = game->Context;

	context->ClearState();

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(game->Display->ClientWidth);
	viewport.Height = static_cast<float>(game->Display->ClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	context->ClearRenderTargetView(gBuffer.AccumulationRTV, Color(0.0f, 0.0f, 0.0f, 1.0f));
	context->ClearRenderTargetView(gBuffer.BloomRTV, Color(0.0f, 0.0f, 0.0f, 1.0f));

	ID3D11RenderTargetView* views[] = { gBuffer.AccumulationRTV, gBuffer.BloomRTV, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	context->OMSetRenderTargets(8, views, game->DepthView);


	//skyBox->Draw(deltaTime);
	skyRend->Draw(deltaTime);

	//float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(blendStateLight, nullptr, 0xffffffff);

	context->VSSetConstantBuffers(0, 1, &constantBuffer);

	context->PSSetConstantBuffers(0, 1, &constantBuffer);
	context->PSSetConstantBuffers(1, 1, &lightsBuffer);
	context->PSSetConstantBuffers(2, 1, &depthRenderer->cascadeBuf);
	context->PSSetConstantBuffers(3, 1, &fogBuffer);

	//context.PixelShader.SetShaderResource(1, Light.DepthResource);
	context->PSSetSamplers(0, 1, &samplerWrap);
	context->PSSetSamplers(1, 1, &samplerClamp);
	context->PSSetSamplers(2, 1, &shadowSampler);

	ID3D11ShaderResourceView* resources[] = { gBuffer.DiffuseSRV, gBuffer.NormalSRV, gBuffer.MetRougAoIdSRV, gBuffer.EmissiveSRV, gBuffer.WorldPositionSRV, preMap->Srv, convMap->Srv, intMap->Srv, depthRenderer->depthSRV };
	context->PSSetShaderResources(0, 9, resources);

	game->DebugAnnotation->BeginEvent(L"Lighting");
	for(auto& light : Lights)
	{
		light->DrawDebugData();

		auto lightData = light->GetLightData();

		context->UpdateSubresource(lightsBuffer, 0, nullptr, &lightData, 0, 0);

		constData.World = light->GetTransformMatrix();
		constData.WorldViewProj = constData.World * constData.View * constData.Proj;

		context->UpdateSubresource(constantBuffer, 0, nullptr, &constData, 0, 0);

		
		if (light->LightType == LightTypes::Ambient || light->LightType == LightTypes::Directional)
		{
			context->RSSetState(rastCullBack);
			context->OMSetDepthStencilState(dsLightingLess, 0);

			context->VSSetShader(LightingVSs[VertexFlagsLighting::SCREEN_QUAD], nullptr, 0);
			context->PSSetShader(LightingPSs[light->GetLightFlags()], nullptr, 0);

			context->IASetInputLayout(nullptr);
			context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			//context.InputAssembler.SetVertexBuffers
			context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

			context->Draw(4, 0);

		}
		else {
			context->RSSetState(rastCullFront);
			context->OMSetDepthStencilState(dsLightingGreater, 0);

			context->VSSetShader(LightingVSs[VertexFlagsLighting::NONE], nullptr, 0);
			context->PSSetShader(LightingPSs[light->GetLightFlags()], nullptr, 0);

			context->IASetInputLayout(layoutLighting);
			context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			unsigned int offset = 0;
			context->IASetVertexBuffers(0, 1, &light->MeshRep->VertexBuffer, &light->MeshRep->Stride, &offset);
			context->IASetIndexBuffer(light->MeshRep->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexed(light->MeshRep->IndexCount, 0, 0);
		}
	}
	game->DebugAnnotation->EndEvent();

}

