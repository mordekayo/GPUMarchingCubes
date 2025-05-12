#include "pch.h"
#include "Game.h"
#include "LightComponent.h"
#include "Camera.h"
#include "Keys.h"
#include "DebugRenderSystem.h"
#include "InputDevice.h"
#include "TinyModelComponent.h"

using namespace DirectX::SimpleMath;


DirectX::SimpleMath::Matrix LightComponent::GetViewProjMatrix() const
{
	return ViewProjMatrix;
}

LightComponent::LightComponent(Game* game) : GameComponent(game)
{
}
 
LightComponent::~LightComponent()
{
}

void LightComponent::Initialize()
{
	D3D11_TEXTURE2D_DESC depthDescription = {}; 
	depthDescription.Width = ShadowMapSize;
	depthDescription.Height = ShadowMapSize;
	depthDescription.ArraySize = 1;
	depthDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthDescription.CPUAccessFlags = 0;
	depthDescription.Format = DXGI_FORMAT_R32_TYPELESS,
	depthDescription.MipLevels = 1,
	depthDescription.MiscFlags = 0,
	depthDescription.SampleDesc.Count = 1;
	depthDescription.SampleDesc.Quality = 0;
	depthDescription.Usage = D3D11_USAGE_DEFAULT,

	game->Device->CreateTexture2D(&depthDescription, nullptr, &resource);


	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc = {};
	dViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	game->Device->CreateDepthStencilView(resource, &dViewDesc, &DepthStencilView);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	
	game->Device->CreateShaderResourceView(resource, &srvDesc, &DepthShaderView);

	
	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthBias = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthBiasClamp = 0;

	game->Device->CreateRasterizerState(&rastDesc, &rastState);
}

Vector3 p = Vector3::Zero;
void LightComponent::Update(float deltaTime)
{
	ViewProjMatrix = Matrix::CreateLookAt(Position, Position + Direction, Up) * Matrix::CreateOrthographic(300, 300, 0.1f, 750.0f); // * Matrix::CreatePerspectiveFieldOfView(3.141593f * 0.25f, 1, 0.1f, 1750.0f);// 

	game->DebugRender->DrawArrow(Position, Position + Direction*100.0f, Color(1.0f, 1.0f, 0.0, 1.0f), Up);


	if (game->InputDevice->IsKeyDown(Keys::Left))	p += Vector3::Left;
	if (game->InputDevice->IsKeyDown(Keys::Right))	p += Vector3::Right;
	if (game->InputDevice->IsKeyDown(Keys::Up))		p += Vector3::Forward;
	if (game->InputDevice->IsKeyDown(Keys::Down))	p += Vector3::Backward;

	game->DebugRender->DrawPoint(Vector3(p.x, p.y, p.z), 10);
}

void LightComponent::Draw(float deltaTime)
{
	auto context = game->Context;

	
	context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(0, nullptr, DepthStencilView);

	context->RSSetState(rastState);

	D3D11_VIEWPORT viewport = {};
	viewport.Width	= ShadowMapSize;
	viewport.Height = ShadowMapSize;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	for(auto* component : game->Components) {
		auto comp = dynamic_cast<TinyModelComponent*>(component);
		if (!comp) continue;
		
		comp->DrawToDepth(this);
	}

	game->RestoreTargets();
}

void LightComponent::DestroyResources()
{
}

void LightComponent::Reload()
{
}

void LightComponent::ReCreateRasterizerState(int DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias)
{
	SafeRelease(rastState);	

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthBias		= DepthBias;
	rastDesc.DepthBiasClamp = DepthBiasClamp;
	rastDesc.SlopeScaledDepthBias = SlopeScaledDepthBias;

	game->Device->CreateRasterizerState(&rastDesc, &rastState);
}
