#include "RenderingSystemPBR.h"
#include "Camera.h"

	void RenderingSystemPBR::OpaquePass(float deltaTime)
	{
		auto context = game->Context;

		context->RSSetState(rastCullBack);
		context->OMSetBlendState(blendStateOpaque, nullptr, 0xffffffff);
		context->OMSetDepthStencilState(depthStateOpaque, 0);

		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->PSSetShader(OpaquePSs[PixelFlagsOpaque::NONE], nullptr, 0);

		context->VSSetConstantBuffers(0, 1, &constantBuffer);

		context->PSSetConstantBuffers(0, 1, &constantBuffer);
		context->PSSetConstantBuffers(1, 1, &materialBuffer);

		//context.PixelShader.SetShaderResource(1, Light.DepthResource);
		context->PSSetSamplers(0, 1, &samplerWrap);
		context->PSSetSamplers(1, 1, &shadowSampler);

		D3D11_VIEWPORT viewport = {};
		viewport.Width = static_cast<float>(game->Display->ClientWidth);
		viewport.Height = static_cast<float>(game->Display->ClientHeight);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;

		context->RSSetViewports(1, &viewport);

		
		context->ClearRenderTargetView(gBuffer.DiffuseRTV, Color(0.0f, 0.0f, 0.0f, 1.0f));
		context->ClearRenderTargetView(gBuffer.NormalRTV, Color(0.0f, 0.0f, 0.0f, 1.0f));
		context->ClearRenderTargetView(gBuffer.MetRougAoIdRTV, Color(0.0f, 0.0f, 0.0f, 1.0f));
		context->ClearRenderTargetView(gBuffer.EmissiveRTV, Color(0.0f, 0.0f, 0.0f, 1.0f));
		context->ClearRenderTargetView(gBuffer.WorldPositionRTV, Color(0.0f, 0.0f, 0.0f, game->GameCamera->FarPlaneDistance));

		ID3D11RenderTargetView* views[] = { gBuffer.DiffuseRTV, gBuffer.NormalRTV, gBuffer.MetRougAoIdRTV, gBuffer.EmissiveRTV, gBuffer.WorldPositionRTV, nullptr, nullptr, nullptr };
		context->OMSetRenderTargets(8, views, game->DepthView);

		game->DebugAnnotation->BeginEvent(L"Opaque Pass");
		for(auto& mesh : Meshes)
		{
			if(mesh->IsSkinned) {
				context->IASetInputLayout(layoutOpaqueAnimated);
				context->VSSetShader(OpaqueVSs[VertexFlagsOpaque::SKINNED_MESH], nullptr, 0);

				context->VSSetShaderResources(0, 1, &mesh->AnimFrame);

			} else {
				context->IASetInputLayout(layoutOpaque);
				context->VSSetShader(OpaqueVSs[VertexFlagsOpaque::NONE], nullptr, 0);

			}

			constData.World = mesh->Transform;
			constData.World.Invert(constData.InvertTransposeWorld);
			constData.InvertTransposeWorld = constData.InvertTransposeWorld.Transpose();
			constData.WorldViewProj = constData.World * constData.View * constData.Proj;

			game->Context->UpdateSubresource(constantBuffer, 0, nullptr, &constData, 0, 0);

			unsigned int offset = 0;
			context->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offset);
			context->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			for (auto& subset : mesh->Subsets)
			{
				auto& mat = mesh->Materials[subset.MaterialIndex];

				MaterialStruct matData{ mat.MetallicMult, mat.RoughnessMult };

				game->Context->UpdateSubresource(materialBuffer, 0, nullptr, &matData, 0, 0);

				context->PSSetShaderResources(0, 1, &mat.DiffuseSRV);
				context->PSSetShaderResources(1, 1, &mat.NormalSRV);
				context->PSSetShaderResources(2, 1, &mat.RoughnessSRV);
				context->PSSetShaderResources(3, 1, &mat.MetallicSRV);

				context->DrawIndexed(subset.PrimitiveCount * 3, subset.StartPrimitive * 3, 0);
			}
			
		}
		game->DebugAnnotation->EndEvent();
	}
