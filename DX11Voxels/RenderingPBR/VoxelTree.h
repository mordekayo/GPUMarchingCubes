#pragma once
#include <DirectXCollision.h>
#include <set>
#include <VGJS.h>


enum class VoxelNodeState
{
	UNINITIALIZED,
	INITIALIZATION,
	EMPTY,
	FULL,
	MIXED
};


const Vector3 VoxelOffsets[] = { 
	Vector3::Up + Vector3::Forward + Vector3::Right,  // VoxelPosition::TopNorthEast, 
	Vector3::Up + Vector3::Backward + Vector3::Right, // VoxelPosition::TopSouthEast, 
	Vector3::Up + Vector3::Backward + Vector3::Left,  //VoxelPosition::TopSouthWest,
	Vector3::Up + Vector3::Forward + Vector3::Left,   //VoxelPosition::TopNorthWest,
	Vector3::Down + Vector3::Forward + Vector3::Right, //VoxelPosition::BotNorthEast,
	Vector3::Down + Vector3::Backward + Vector3::Right,//VoxelPosition::BotSouthEast,
	Vector3::Down + Vector3::Backward + Vector3::Left, //VoxelPosition::BotSouthWest,
	Vector3::Down + Vector3::Forward + Vector3::Left,  //VoxelPosition::BotNorthWest 
};

struct VoxelNode
{
	VoxelNodeState state;

	float* data		= nullptr;
	uint64_t depth	= 0;
	uint64_t childIndex = 0;

	VoxelNode* parent;
	VoxelNode** childs = nullptr;

	VoxelNode(VoxelNode* parent);

	bool isDirty = true;
	bool isMeshCalculated = false;
	bool isLastInLOD = false;

	DirectX::BoundingBox aabb;
	double x = 0, y = 0, z = 0, extend = 0;

	ID3D11Texture3D*			volumeTex = nullptr;
	ID3D11ShaderResourceView*	volumeSRV = nullptr;
	ID3D11UnorderedAccessView*	volumeUAV = nullptr;

	ID3D11Buffer*				appendBuffer	= nullptr;
	ID3D11UnorderedAccessView*	appendUAV		= nullptr;
	ID3D11ShaderResourceView*	appendSRV		= nullptr;

	ID3D11Buffer*				indirectBuffer	= nullptr;
	ID3D11UnorderedAccessView*	indirectUAV		= nullptr;


	bool HaveChilds() 
	{
		return childs != nullptr;
	}

	~VoxelNode();
};


struct QueryRes
{
	std::set<VoxelNode*> nodes;

	void AddNode(VoxelNode* node);
};


struct VoxelTree
{
#pragma region RenderStuff
	ID3D11VertexShader* pointVolVS = nullptr;
	ID3D11GeometryShader* pointVolGS = nullptr;
	ID3D11PixelShader* pointVolPS = nullptr;

	ID3D11ComputeShader* updateVolCS = nullptr;
	ID3D11ComputeShader* sdfFillVolCS = nullptr;
	ID3D11ComputeShader* marchingCubeCS = nullptr;
	ID3D11ComputeShader* fillIndirectCS = nullptr;

	ID3D11VertexShader* marchingRendVS = nullptr;
	ID3D11VertexShader* marchingRendBufVS = nullptr;
	ID3D11PixelShader* marchingRendPS = nullptr;
	ID3D11InputLayout* marchingLayout = nullptr;

	ID3D11VertexShader* VSRenderToShadowMap = nullptr;
	ID3D11GeometryShader* GSRenderToShadowMap = nullptr;

	ID3D11SamplerState* TrilinearClamp;

	ID3D11ShaderResourceView* defaultlookUpTableSRV = nullptr;
	ID3D11ShaderResourceView* lookUpTable13PointsSRV = nullptr;
	ID3D11ShaderResourceView* lookUpTable17PointsSRV = nullptr;
	ID3D11ShaderResourceView* lookUpTable20PointsSRV = nullptr;

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
		DirectX::SimpleMath::Vector4 CornerPosWorldSize;
		DirectX::SimpleMath::Vector4 TransitionParams;
		DirectX::SimpleMath::Vector4 TransitionParams2;
	};

	ID3D11Buffer* constPointVolBuf = nullptr;
	ID3D11Buffer* constComputeBuf = nullptr;

	PointVolConstParams	constData;
	ComputeConstParams	computeConstData;
#pragma endregion RenderStuff


	VoxelNode* rootNode = nullptr;

	uint64_t minNodeSizeInMeters = 16;
	uint64_t maxDepth = 8;
	uint32_t voxelSize = 16;

	float falloff = 2.0f;

	QueryRes lastQuery;

	const Camera& camera;
	const Game& game;

	vgjs::JobSystem js;


	std::vector<float> lods;

	std::vector<std::chrono::microseconds> last100ComputeTimes;
	std::vector<std::chrono::microseconds> last100DrawTimes;
public:
	VoxelTree(const Camera& inCamera, const Game& game);

	uint64_t CalculateSizeByDepth(uint64_t depth);

	bool NeedToSplit(VoxelNode* node, const Camera& camera);
	QueryRes CullTree(const Camera& camera);

	void GrowBranch(VoxelNode* leaf);
	void CutBranch(VoxelNode* branch);

	void InitializeNodeParams(VoxelNode* node, uint32_t nodeIndex);

	void InitializeVolume(VoxelNode* node);
	
	bool IsNodeInCache();
	bool IsNodeInMemoryCache();
	bool IsNodeInFileCahce();

	void Update();
	void Draw();
	void Draw2();

	void DrawDebug();

	void AddSdfSphere(DirectX::SimpleMath::Vector3 spherePos, float radius, bool isSub = false);
	void AddSdfSphere(VoxelNode* node, DirectX::SimpleMath::Vector3 spherePos, float radius, bool isSub = false);


	void CheckTransitionStateAndCalculateMarchingCubes(VoxelNode* node);
	void CalculateMarchingCubes(VoxelNode* node, int lowResNeighboursMask);
	void LoadShaders();
	void CreateTriangulationLookUpTables();
	ID3D11ShaderResourceView* CreateLookUpTable(int vertexCount, int edgesCount, const void const* data, bool truncate);

	bool once = false;
};

