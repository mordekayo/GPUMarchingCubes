#include "Game.h"
#include "IRenderer2D.h"
#include "Camera.h"
#include "DebugRenderSystem.h"
#include "VoxelTree.h"
#include <queue>
#include "Noise/SimplexNoise.h"
#include <unordered_set>



VoxelTree::VoxelTree(const Camera& inCamera) : camera(inCamera), js(vgjs::thread_count_t{8})
{
	LoadShaders();

	lods = {100.0f, 300.0f, 800.0f, 2000.0f, 4000.0f, 8000.0f, 9000.0, 10000.0f};

	rootNode = new VoxelNode(nullptr);
	InitializeNodeParams(rootNode, 0);
	InitializeVolume(rootNode);

	//GrowBranch(rootNode);
	//for(int i = 0; i < 8; ++i)
	//	InitializeVolume(rootNode->childs[i]);


	lastQuery.AddNode(rootNode);
}


uint64_t VoxelTree::CalculateSizeByDepth(uint64_t depth)
{
	if (depth > maxDepth)
		return minNodeSizeInMeters;

	uint64_t size = (2ull << (maxDepth-depth)) * minNodeSizeInMeters;
	return size;
}


// Given point p, return the point q on or in AABB b that is closest to p
void ClosestPtPointAABB(Vector3 p, DirectX::BoundingBox b, Vector3& q)
{
	Vector3 min = b.Center - b.Extents;
	Vector3 max = b.Center + b.Extents;
	// For each coordinate axis, if the point coordinate value is
	// outside box, clamp it to the box, else keep it as is
	float x = p.x;
	if (x < min.x) x = min.x; // v = max(v, b.min[i])
	if (x > max.x) x = max.x; // v = min(v, b.max[i])
	q.x = x;

	float y = p.y;
	if (y < min.y) y = min.y; // v = max(v, b.min[i])
	if (y > max.y) y = max.y; // v = min(v, b.max[i])
	q.y = y;

	float z = p.z;
	if (z < min.z) z = min.z; // v = max(v, b.min[i])
	if (z > max.z) z = max.z; // v = min(v, b.max[i])
	q.z = z;
}


bool VoxelTree::NeedToSplit(VoxelNode* node, const Camera& camera)
{
	//return node->depth != 2;

	float nearPlane = camera.NearPlaneDistance;
	float farPlane	= camera.FarPlaneDistance;

	Vector3 nearPoint;
	ClosestPtPointAABB(camera.GetPosition(), node->aabb, nearPoint);
	Vector3 localPos = nearPoint - camera.GetPosition();
	float dist = localPos.Length();

	for(int i = 0; i < maxDepth; ++i) {
		float lodDist = lods[i];
		if(dist < lodDist && node->depth < (8-i))
			return true;
	}

	return false;

	//float norm = 1.0f - std::clamp((dist - nearPlane) / (farPlane - nearPlane), 0.0f, 1.0f);
	//
	//norm = std::pow(norm, falloff);
	//
	//uint64_t expectedDepth = norm * double(maxDepth);
	//
	//if (node->depth < expectedDepth)
	//	return true;
	//
	//return false;
}

QueryRes VoxelTree::CullTree(const Camera& camera)
{
	//Game::Instance->DebugRender->DrawFrustrum(camera.ViewMatrix, camera.ProjMatrix);

	DirectX::BoundingFrustum frustum = DirectX::BoundingFrustum(camera.ProjMatrix, true);
	DirectX::BoundingFrustum trFrustum;

	DirectX::SimpleMath::Matrix inv;
	camera.ViewMatrix.Invert(inv);

	frustum.Transform(trFrustum, inv);


	QueryRes res;


	std::queue<VoxelNode*> traverseNodes;
	traverseNodes.push(rootNode);

	while (!traverseNodes.empty())
	{
		VoxelNode* node = traverseNodes.front();
		traverseNodes.pop();

		if (trFrustum.Contains(node->aabb) == 0) {
			//if (node->HaveChilds())
			//	CutBranch(node);
			continue;
		}

		if (node->depth >= maxDepth) {
			res.AddNode(node);
			continue;
		}

		if (NeedToSplit(node, camera)) {
			GrowBranch(node);
			for (int i = 0; i < 8; ++i)
				traverseNodes.push(node->childs[i]);
		}
		else {
			res.AddNode(node);
			continue;
		}
	}

	return res;
}



void VoxelTree::InitializeVolume(VoxelNode* node)
{
	if (node->state != VoxelNodeState::UNINITIALIZED && node->state != VoxelNodeState::INITIALIZATION)
		return;

	SafeRelease(node->volumeTex);
	SafeRelease(node->volumeSRV);
	SafeRelease(node->volumeUAV);

	float nodeSize = CalculateSizeByDepth(node->depth);

	Vector3 voxelCorner = node->aabb.Center - node->aabb.Extents;

	bool havePositive = false;
	bool haveNegative = false;
	float* data = new float[voxelSize * voxelSize * voxelSize];
		for (unsigned int z = 0; z < voxelSize; ++z) {
			for (unsigned int y = 0; y < voxelSize; ++y) {
				for (unsigned int x = 0; x < voxelSize; ++x) {
					Vector3 pos = voxelCorner + Vector3(float(x) / float(voxelSize - 1), float(y) / float(voxelSize - 1), float(z) / float(voxelSize - 1)) * node->aabb.Extents.y * 2;
					float height = pos.y - SimplexNoise::noise(pos.x * 0.002, pos.z * 0.002)*100 + 10 + SimplexNoise::noise(pos.x * 0.005, pos.z * 0.005) * 50 + +SimplexNoise::noise(pos.x * 0.00025, pos.z * 0.00025) * 500;
					if (height >= 0.0f) havePositive = true;
					if (height <  0.0f) haveNegative = true;
					data[x + y * voxelSize + z * voxelSize * voxelSize] = height;
			}
		}
	}

	D3D11_SUBRESOURCE_DATA dataSource;
	dataSource.SysMemSlicePitch = voxelSize * voxelSize * sizeof(float);
	dataSource.SysMemPitch = voxelSize * sizeof(float);
	dataSource.pSysMem = data;


	D3D11_TEXTURE3D_DESC texDesc{
		voxelSize, voxelSize, voxelSize,
		1,
		DXGI_FORMAT_R32_FLOAT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		0, 0
	};

	Game::Instance->Device->CreateTexture3D(&texDesc, &dataSource, &node->volumeTex);
	Game::Instance->Device->CreateShaderResourceView(node->volumeTex, nullptr, &node->volumeSRV);
	Game::Instance->Device->CreateUnorderedAccessView(node->volumeTex, nullptr, &node->volumeUAV);

	delete[] data;

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	unsigned int cubesCount = (voxelSize - 1) * (voxelSize - 1) * (voxelSize - 1);
	UINT structSize = sizeof(Vector3) * 7;
	D3D11_BUFFER_DESC vertBufDesc{
		.ByteWidth = 5 * cubesCount * structSize, // 5 is max triangles in cube
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0,
		.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		.StructureByteStride = structSize
	};

	Game::Instance->Device->CreateBuffer(&vertBufDesc, nullptr, &node->appendBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{
		.Format = DXGI_FORMAT_UNKNOWN,
		.ViewDimension = D3D11_UAV_DIMENSION_BUFFER,
		.Buffer = D3D11_BUFFER_UAV {0, 5 * cubesCount, D3D11_BUFFER_UAV_FLAG_APPEND}
	};
	Game::Instance->Device->CreateUnorderedAccessView(node->appendBuffer, &uavDesc, &node->appendUAV);
	Game::Instance->Device->CreateShaderResourceView(node->appendBuffer, nullptr, &node->appendSRV);


	D3D11_BUFFER_DESC indirectBufDesc{
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

	UINT indData[] = { 0,1,0,0 };
	D3D11_SUBRESOURCE_DATA indirectSource;
	indirectSource.SysMemSlicePitch = 0;
	indirectSource.SysMemPitch = 0;
	indirectSource.pSysMem = indData;

	Game::Instance->Device->CreateBuffer(&indirectBufDesc, &indirectSource, &node->indirectBuffer);
	Game::Instance->Device->CreateUnorderedAccessView(node->indirectBuffer, &indirectUavDesc, &node->indirectUAV);


	if (havePositive && !haveNegative)
		node->state = VoxelNodeState::EMPTY;
	else if (!havePositive && haveNegative)
		node->state = VoxelNodeState::FULL;
	else
		node->state = VoxelNodeState::MIXED;
}


void VoxelTree::Update()
{
	lastQuery = CullTree(camera);

	for (auto* node : lastQuery.nodes)
	{
		if (node->state == VoxelNodeState::UNINITIALIZED) {
			node->state = VoxelNodeState::INITIALIZATION;
			vgjs::schedule([node, this](){ InitializeVolume(node); });
		}

		if (node->state == VoxelNodeState::MIXED && !node->isMeshCalculated)
			CalculateMarchingCubes(node);
	}

	constData.World = Matrix::Identity;
	constData.View = camera.ViewMatrix;
	constData.Projection = camera.ProjMatrix;
}


void VoxelTree::GrowBranch(VoxelNode* node)
{
	if (node == nullptr)
		return;

	if (node->childs != nullptr)
		return;

	node->childs = new VoxelNode * [8];

	for (int i = 0; i < 8; ++i) {
		node->childs[i] = new VoxelNode(node);
		InitializeNodeParams(node->childs[i], i);
	}
}


void VoxelTree::CutBranch(VoxelNode* node)
{
	if (node == nullptr)
		return;
	if (node->childs == nullptr)
		return;

	delete[] node->childs;
	node->childs = nullptr;
}


void VoxelTree::DrawDebug()
{
	std::wstringstream strstr;

	strstr << "Visible nodes count: " << lastQuery.nodes.size() << "\n";
	std::wstring str = strstr.str();

	Game::Instance->Renderer2D->DrawOnScreenMessage(str);
	Game::Instance->DebugRender->DrawBoundingBox(rootNode->aabb);
	//for (const auto* n : lastQuery.nodes) {
	//	Game::Instance->DebugRender->DrawBoundingBox(n->aabb);
	//}

}


void VoxelTree::InitializeNodeParams(VoxelNode* node, uint32_t nodeIndex)
{
	node->depth = node->parent ? node->parent->depth + 1 : 0;
	double halfNodeSize = CalculateSizeByDepth(node->depth) * 0.5;

	Vector3 pos = Vector3::Zero;

	if (node->parent) { 
		Vector3 offset = VoxelOffsets[nodeIndex];
		pos = node->parent->aabb.Center + offset * halfNodeSize;

		node->x = node->parent->x + static_cast<double>(offset.x) * halfNodeSize;
		node->y = node->parent->y + static_cast<double>(offset.y) * halfNodeSize;
		node->z = node->parent->z + static_cast<double>(offset.z) * halfNodeSize;
	}
	node->extend = halfNodeSize;
	node->aabb	= DirectX::BoundingBox(pos, Vector3(static_cast<float>(halfNodeSize), static_cast<float>(halfNodeSize), static_cast<float>(halfNodeSize)));
	node->state = VoxelNodeState::UNINITIALIZED;
}


void QueryRes::AddNode(VoxelNode* node)
{
	nodes.insert(node);
}


VoxelNode::VoxelNode(VoxelNode* inParent) : parent(inParent)
{

}


VoxelNode::~VoxelNode()
{
	if (childs) {
		for (int i = 0; i < 8; ++i) {
			delete childs[i];
		}
		delete[] childs;
	}

	// free video memory
	SafeRelease(volumeTex	  );
	SafeRelease(volumeSRV	  );
	SafeRelease(volumeUAV	  );
	SafeRelease(appendBuffer  );
	SafeRelease(appendUAV	  );
	SafeRelease(appendSRV	  );
	SafeRelease(indirectBuffer);
	SafeRelease(indirectUAV	  );
}

void VoxelTree::CalculateMarchingCubes(VoxelNode* node)
{
	auto ctx = Game::Instance->Context;

	computeConstData.Offset = Vector4(0,0,0, 0);

	ctx->CSSetConstantBuffers(0, 1, &constComputeBuf);
	ctx->CSSetShader(marchingCubeCS, nullptr, 0);
	ctx->CSSetSamplers(0, 1, &TrilinearClamp);

	Vector3 nodePosition = node->aabb.Center;
	computeConstData.PositionIsoline = Vector4(0,0,0, node->depth);
	ctx->UpdateSubresource(constComputeBuf, 0, nullptr, &computeConstData, 0, 0);

	int indBufData[] = { 0, 1, 0, 0 };
	ctx->UpdateSubresource(node->indirectBuffer, 0, nullptr, indBufData, 0, 0);

	UINT counts[1] = { 0 };
	ctx->CSSetUnorderedAccessViews(0, 1, &node->appendUAV, counts);
	ctx->CSSetUnorderedAccessViews(1, 1, &node->indirectUAV, counts);
	ctx->CSSetShaderResources(0, 1, &node->volumeSRV);

	const UINT groupCount = static_cast<UINT>(std::ceil(16.0f / 8.0f));
	ctx->Dispatch(groupCount, groupCount, groupCount);

	ID3D11UnorderedAccessView* nullViews[] = { nullptr, nullptr };
	UINT nullCounts[] = { 0, 0 };
	ctx->CSSetUnorderedAccessViews(0, 2, nullViews, nullCounts);

	static int t = 0;
	//std::cout << "cmc "<< t++ << "\n";

	node->isMeshCalculated = true;
}

void VoxelTree::Draw2()
{
	auto ctx = Game::Instance->Context;

	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	ctx->VSSetShader(pointVolVS, nullptr, 0);
	ctx->GSSetShader(pointVolGS, nullptr, 0);
	ctx->PSSetShader(pointVolPS, nullptr, 0);

	ctx->GSSetConstantBuffers(0, 1, &constPointVolBuf);

	for (auto* node : lastQuery.nodes) {
		const auto& chunk = *node;

		if (chunk.state != VoxelNodeState::MIXED)
			continue;
		if (chunk.depth != maxDepth)
			continue;

		ctx->GSSetShaderResources(0, 1, &chunk.volumeSRV);

		constData.PositionSize = Vector4((float)(chunk.x - chunk.extend), (float)(chunk.y - chunk.extend), (float)(chunk.z - chunk.extend), 16);
		constData.ScaleParticleSizeThreshold = Vector4(chunk.extend*2, 1, 0, 0);
		ctx->UpdateSubresource(constPointVolBuf, 0, nullptr, &constData, 0, 0);

		ctx->Draw(voxelSize * voxelSize * voxelSize, 0);
	}
	
	ctx->VSSetShader(nullptr, nullptr, 0);
	ctx->GSSetShader(nullptr, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);
}

void VoxelTree::Draw()
{
	auto ctx = Game::Instance->Context;

	Game::Instance->DebugAnnotation->BeginEvent(L"RenderVoxels");

	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ctx->VSSetShader(marchingRendVS, nullptr, 0);
	ctx->PSSetShader(marchingRendPS, nullptr, 0);

	ctx->VSSetConstantBuffers(0, 1, &constPointVolBuf);

	int visCount = 0;
	for (auto* node : lastQuery.nodes) {
		const auto& chunk = *node;

		if(chunk.state != VoxelNodeState::MIXED)
			continue;
		//if (chunk.depth != maxDepth)
		//	continue;
		
		visCount++;
		Vector3 nodePosition = chunk.aabb.Center - chunk.aabb.Extents;
		constData.PositionSize = Vector4(nodePosition, voxelSize);
		constData.ScaleParticleSizeThreshold = Vector4(chunk.aabb.Extents.x*2, 1, 0, 0);
		ctx->UpdateSubresource(constPointVolBuf, 0, nullptr, &constData, 0, 0);
	
		ctx->VSSetShaderResources(0, 1, &chunk.appendSRV);
		ctx->DrawInstancedIndirect(chunk.indirectBuffer, 0);
	}

	ctx->VSSetShader(nullptr, nullptr, 0);
	ctx->GSSetShader(nullptr, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);

	Game::Instance->DebugAnnotation->EndEvent();

	std::wstringstream strstr;
	strstr << "Renderable nodes count: " << visCount << "\n";
	std::wstring str = strstr.str();

	Game::Instance->Renderer2D->DrawOnScreenMessage(str);
}

void VoxelTree::LoadShaders()
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
	constData.ScaleParticleSizeThreshold = Vector4(1, 0.5f, 0.0f, 0.0f);

	constVolDesc.ByteWidth = sizeof(ComputeConstParams);
	Game::Instance->Device->CreateBuffer(&constVolDesc, nullptr, &constComputeBuf);
	computeConstData.SizeXYZScale = Vector4(static_cast<float>(voxelSize), static_cast<float>(voxelSize), static_cast<float>(voxelSize), 1.0f);

}



bool SphereAABBTest(double Bmin[3], double Bmax[3], double C[3], double r2)
{
	double dmin = 0;
	for (int i = 0; i < 3; i++) {
		if (C[i] < Bmin[i]) dmin += std::sqrt(C[i] - Bmin[i])* std::sqrt(C[i] - Bmin[i]); else
			if (C[i] > Bmax[i]) dmin += std::sqrt(C[i] - Bmax[i])* std::sqrt(C[i] - Bmax[i]);
	}
	return dmin <= r2;
}


void VoxelTree::AddSdfSphere(DirectX::SimpleMath::Vector3 spherePos, float radius, bool isSub)
{
	DirectX::BoundingSphere sphere(spherePos, radius*2);


	std::queue<VoxelNode*> traverseNodes;
	traverseNodes.push(rootNode);

	std::unordered_set<VoxelNode*> set;

	while (!traverseNodes.empty())
	{
		VoxelNode* node = traverseNodes.front();
		traverseNodes.pop();

		if (node->aabb.Contains(sphere) == 0) {
			//if (node->HaveChilds())
			//	CutBranch(node);
			continue;
		}

		//double bmin[] = { node->x - node->extend, node->y - node->extend, node->z - node->extend };
		//double bmax[] = { node->x + node->extend, node->y + node->extend, node->z + node->extend };
		//double C[] = { spherePos.x, spherePos.y, spherePos.z };
		//if (!SphereAABBTest(bmin, bmax, C, static_cast<double>(radius)* static_cast<double>(radius)))
		//{
		//	continue;
		//}

		set.insert(node);

		if (node->depth >= maxDepth) {
			continue;
		}

		GrowBranch(node);
		for (int i = 0; i < 8; ++i)
			traverseNodes.push(node->childs[i]);
	}

	for (auto* node : set) {

		if(node->state == VoxelNodeState::UNINITIALIZED)
			InitializeVolume(node);

		node->state = VoxelNodeState::MIXED;
		AddSdfSphere(node, spherePos, radius, isSub);
	}
}

void VoxelTree::AddSdfSphere(VoxelNode* node, DirectX::SimpleMath::Vector3 spherePos, float radius, bool isSub)
{
	auto ctx = Game::Instance->Context;
	Vector3 camPos = camera.GetPosition();
	computeConstData.Offset = Vector4(spherePos - camPos, radius);

	ctx->CSSetConstantBuffers(0, 1, &constComputeBuf);
	ctx->CSSetShader(sdfFillVolCS, nullptr, 0);

	double corX = node->x - node->extend - static_cast<double>(camPos.x);
	double corY = node->y - node->extend - static_cast<double>(camPos.y);
	double corZ = node->z - node->extend - static_cast<double>(camPos.z);
	
	computeConstData.PositionIsoline = Vector4(0, 0, 0, isSub);
	computeConstData.CornerPosWorldSize = Vector4(corX, corY, corZ, node->extend * 2);
	ctx->UpdateSubresource(constComputeBuf, 0, nullptr, &computeConstData, 0, 0);

	UINT counts[1] = { static_cast<UINT>(-1) };
	ctx->CSSetUnorderedAccessViews(0, 1, &node->volumeUAV, &counts[0]);

	const UINT groupCount = static_cast<UINT>(std::ceil(voxelSize / 8.0f));
	ctx->Dispatch(groupCount, groupCount, groupCount);
	

	CalculateMarchingCubes(node);
}
