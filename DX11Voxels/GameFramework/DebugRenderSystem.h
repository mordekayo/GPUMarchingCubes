#pragma once

#include "Exports.h"
#include "../ZMathLib/SimpleMath.h"
#include "d3d11.h"

class StaticMesh;
class Camera;



class GAMEFRAMEWORK_API DebugRenderSystem
{


public:
	virtual ~DebugRenderSystem() = default;


	virtual void SetCamera(Camera* inCamera) = 0;

	virtual void Draw(float dTime) = 0;
	virtual void Clear() = 0;

	virtual void DrawBoundingBox(const DirectX::BoundingBox& box) = 0;
	virtual void DrawBoundingBox(const DirectX::BoundingBox& box, const DirectX::SimpleMath::Matrix& transform) = 0;
	virtual void DrawLine(const DirectX::SimpleMath::Vector3& pos0, const DirectX::SimpleMath::Vector3& pos1, const DirectX::SimpleMath::Color& color) = 0;
	virtual void DrawArrow(const DirectX::SimpleMath::Vector3& p0, const DirectX::SimpleMath::Vector3& p1, const DirectX::SimpleMath::Color& color, const DirectX::SimpleMath::Vector3& n) = 0;
	virtual void DrawPoint(const DirectX::SimpleMath::Vector3& pos, const float& size) = 0;
	virtual void DrawCircle(const double& radius, const DirectX::SimpleMath::Color& color, const DirectX::SimpleMath::Matrix& transform, int density = 40) = 0;
	virtual void DrawSphere(const double& radius, const DirectX::SimpleMath::Color& color, const DirectX::SimpleMath::Matrix& transform, int density = 40) = 0;

	virtual void DrawFrustrum(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) = 0;

	// Plane is P=<Nx,Ny,Nz, D>, where D=-N*P, and P is a point on plane
	virtual void DrawPlane(const DirectX::SimpleMath::Vector4& p, const DirectX::SimpleMath::Color& color, float sizeWidth = 40, float sizeNormal = 1, bool drawCenterCross = false) = 0;

	virtual void DrawTextureOnScreen(ID3D11ShaderResourceView* tex, int x, int y, int width, int height, int zOrder = 1) = 0;

	virtual void DrawStaticMesh(const StaticMesh& mesh, const DirectX::SimpleMath::Matrix& transform, const DirectX::SimpleMath::Color& color) = 0;
};

