#include "RenderingSystemPBR.h"

void RenderingSystemPBR::ToneMapPass(float deltaTime)
{
	game->RestoreTargets();
	game->DebugAnnotation->BeginEvent(L"ToneMap Pass");

	auto context = game->Context;

	context->RSSetState(rastCullBack);
	context->OMSetBlendState(blendStateOpaque, nullptr, 0xffffffff);
	context->OMSetDepthStencilState(dsNoDepth, 0);

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	context->VSSetShader(ToneVSs[VSToneMapFlags::NONE], nullptr, 0);
	context->PSSetShader(TonePSs[ToneFlagPs], nullptr, 0);

	//constData.World = mesh.Transform;
	//constData.WorldViewProj = constData.World * constData.View * constData.Proj;

	context->UpdateSubresource(constantToneBuffer, 0, nullptr, &Luminance, 0,0);

	context->PSSetConstantBuffers(0, 1, &constantToneBuffer);

	context->PSSetShaderResources(0, 1, &gBuffer.AccumulationSRV);
	context->PSSetShaderResources(1, 1, &gBuffer.BloomSRV);


	//context.InputAssembler.SetVertexBuffers(0, null, null, null);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

	context->Draw(4, 0);

	game->DebugAnnotation->EndEvent();
}
