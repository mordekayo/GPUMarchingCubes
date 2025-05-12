#pragma once
#include "d3d11.h"
#include "DebugRenderSystem.h"
#include "VertexPositionColor.h"

class Game;
class Camera;


class DebugRenderSysImpl : public DebugRenderSystem
{
	friend class Game;
	
	Game*		game = nullptr;
	Camera*		camera = nullptr;
	ID3D11Buffer* constBuf;

	DebugRenderSysImpl(Game* inGame);


#pragma region Primitives
	std::vector<VertexPositionColor> lines;

	ID3D11VertexShader* vertexPrimShader;
	ID3D11PixelShader*	pixelPrimShader;
	ID3DBlob* vertexPrimCompResult;
	ID3DBlob* pixelPrimCompResult;

	ID3D11InputLayout*	layoutPrim;
	ID3D11Buffer*		verticesPrim;
	UINT bufPrimStride;

	ID3D11RasterizerState*		rastState;
	ID3D11DepthStencilState*	depthState;
	bool isPrimitivesDirty = false;

	const int MaxPointsCount = 1024 * 1024 * 8;
	int pointsCount = 0;
#pragma endregion Primitives

#pragma region Quads

	DirectX::SimpleMath::Matrix quadProjMatrix;
	
	struct QuadInfo
	{
		ID3D11ShaderResourceView* Srv;
		DirectX::SimpleMath::Matrix TransformMat;
	};

	std::vector<QuadInfo> quads;

	const UINT QuadMaxDrawCount = 100;


	ID3D11Buffer* quadBuf;
	UINT quadBindingStride;

	ID3D11PixelShader*	pixelQuadShader;
	ID3D11VertexShader*	vertexQuadShader;
	ID3DBlob* pixelQuadCompResult;
	ID3DBlob* vertexQuadCompResult;

	ID3D11InputLayout*	quadLayout;
	ID3D11SamplerState* quadSampler;

	ID3D11RasterizerState* quadRastState;

#pragma endregion Quads

#pragma region Meshes

	struct MeshInfo
	{
		const StaticMesh* Mesh;
		DirectX::SimpleMath::Vector4 Color;
		DirectX::SimpleMath::Matrix Transform;
	};


	struct MeshConstData
	{
		DirectX::SimpleMath::Matrix Transform;
		DirectX::SimpleMath::Vector4 Color;
	};

	std::vector<MeshInfo> meshes;

	ID3D11VertexShader* vertexMeshShader;
	ID3D11PixelShader* pixelMeshShader;
	ID3DBlob* pixelMeshCompResult;
	ID3DBlob* vertexMeshCompResult;

	ID3D11InputLayout* meshLayout;

	ID3D11Buffer* meshBuf;

#pragma endregion Meshes
	
protected:

	void InitPrimitives();
	void InitQuads();
	void InitMeshes();
	
	void DrawPrimitives();;
	void DrawQuads();
	void DrawMeshes();

	void UpdateLinesBuffer();
	
public:
	virtual ~DebugRenderSysImpl() override;
	virtual void SetCamera(Camera* inCamera) override;
	virtual void Draw(float dTime) override;
	virtual void Clear() override;

public:
	virtual void DrawBoundingBox(const DirectX::BoundingBox& box) override;
	virtual void DrawBoundingBox(const DirectX::BoundingBox& box, const DirectX::SimpleMath::Matrix& transform) override;
	virtual void DrawLine(const DirectX::SimpleMath::Vector3& pos0, const DirectX::SimpleMath::Vector3& pos1, const DirectX::SimpleMath::Color& color) override;
	virtual void DrawArrow(const DirectX::SimpleMath::Vector3& p0, const DirectX::SimpleMath::Vector3& p1, const DirectX::SimpleMath::Color& color, const DirectX::SimpleMath::Vector3& n) override;
	virtual void DrawPoint(const DirectX::SimpleMath::Vector3& pos, const float& size) override;
	virtual void DrawCircle(const double& radius, const DirectX::SimpleMath::Color& color, const DirectX::SimpleMath::Matrix& transform, int density) override;
	virtual void DrawSphere(const double& radius, const DirectX::SimpleMath::Color& color, const DirectX::SimpleMath::Matrix& transform, int density) override;
	virtual void DrawPlane(const DirectX::SimpleMath::Vector4& p, const DirectX::SimpleMath::Color& color, float sizeWidth, float sizeNormal, bool drawCenterCross) override;

	virtual void DrawFrustrum(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

	virtual void DrawTextureOnScreen(ID3D11ShaderResourceView* tex, int x, int y, int width, int height, int zOrder) override;

	virtual void DrawStaticMesh(const StaticMesh& mesh, const DirectX::SimpleMath::Matrix& transform, const DirectX::SimpleMath::Color& color) override;
};

