#include "pch.h"
#include "StaticMesh.h"
#include <filesystem>
#include "FbxLoaderExt.h"
#include "VertexPositionNormalBinormalTangentColorTex.h"


void ReloadTex(const std::wstring& texPath, ID3D11Texture2D*& Tex, ID3D11ShaderResourceView*& TexSrv, bool generateMipmaps = false, bool useSrgb = false)
{
	auto path = std::filesystem::path(texPath);
	
	if (std::filesystem::exists(texPath) && path.extension() != ".tga")
	{
		if (Tex) Tex->Release();
		Tex = nullptr;
		if (TexSrv) TexSrv->Release();
		TexSrv = nullptr;
	
		Game::Instance->TextureLoader->LoadTextureFromFile(texPath.c_str(), Tex, TexSrv, generateMipmaps, useSrgb);
	}
}

void ZMaterial::ReloadTextures(const std::wstring& diffuseMap, const std::wstring& normalMap, const std::wstring& roughMap, const std::wstring& metallicMap)
{
	ReloadTex(diffuseMap, DiffuseMap, DiffuseSRV, true, true);
	ReloadTex(normalMap, NormalMap, NormalSRV, false, false);
	ReloadTex(roughMap, RoughnessMap, RoughnessSRV, true, false);
	ReloadTex(metallicMap, MetallicMap, MetallicSRV, true, false);
}

void ZMaterial::ReloadTextures(const std::string& diffuseMap, const std::string& normalMap, const std::string& roughMap, const std::string& metallicMap)
{
	auto diffW = std::wstring(diffuseMap.begin(), diffuseMap.end());
	auto normW = std::wstring(normalMap.begin(), normalMap.end());
	auto rougW = std::wstring(roughMap.begin(), roughMap.end());
	auto metaW = std::wstring(metallicMap.begin(), metallicMap.end());

	ReloadTex(diffW, DiffuseMap, DiffuseSRV, true, true);
	ReloadTex(normW, NormalMap, NormalSRV, false, false);
	ReloadTex(rougW, RoughnessMap, RoughnessSRV, true, false);
	ReloadTex(metaW, MetallicMap, MetallicSRV, true, false);
}

void ZMaterial::ReloadMetallic(const std::wstring& metallicTex)
{
	ReloadTex(metallicTex, MetallicMap, MetallicSRV, false, false);
}


StaticMesh::StaticMesh(Game* inGame, const std::string& meshName)
{
	game = inGame;
	Transform = DirectX::SimpleMath::Matrix::Identity;
	LocalTransform = DirectX::SimpleMath::Matrix::Identity;

	Reload(meshName);
}


StaticMesh::StaticMesh(Game* inGame, const std::string& meshName, ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer,
                       int vertCount, int indCount, int stride)
{
	game = inGame;
	MeshName = std::move(meshName);

	Transform = DirectX::SimpleMath::Matrix::Identity;
	LocalTransform = DirectX::SimpleMath::Matrix::Identity;

	ReloadMesh(vertexBuffer, indexBuffer, vertCount, indCount, stride);
}


void StaticMesh::ReloadMesh(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int vertCount, int indCount, int stride)
{
	if (VertexBuffer) VertexBuffer->Release();
	if (IndexBuffer) IndexBuffer->Release();

	VertexBuffer = vertexBuffer;
	IndexBuffer = indexBuffer;

	Stride = stride;

	VertexCount = vertCount;
	IndexCount = indCount;
}


void StaticMesh::Reload(std::string meshName)
{
	
	try {
		auto scene = game->FbxLoader->LoadScene(meshName);

		if (scene->Meshes.size() > 1) std::cout << "Warning ";
		
		auto mesh = scene->Meshes[0];

		std::vector<VertexPositionNormalBinormalTangentColorTex> verts;
		std::vector<int> inds;
		
		ConvertStaticMeshToVertices(mesh, verts, inds);
		//Convert scene to buffers
		
		//ReloadMesh
		//ReloadTextures
	}
	catch (std::exception& ex) {
		std::cout	<< "Error while loading model: " << meshName << "\n\t";
		std::cout	<< ex.what();
		return;
	}
	
	MeshName = meshName;
	
	Stride = VertexPositionNormalBinormalTangentColorTex::Stride;
}


void StaticMesh::InitSkeletalData()
{
	if (scene == nullptr) return;

	std::vector<Matrix> source;
	source.resize(scene->Nodes.size());
	std::vector<Matrix> dest;
	dest.resize(scene->Nodes.size());

	scene->GetAnimSnapshot(0, source);
	scene->ComputeBoneTransforms(source, dest);

	D3D11_BUFFER_DESC bindBufDesc = {};
	bindBufDesc.Usage = D3D11_USAGE_DEFAULT;
	bindBufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bindBufDesc.CPUAccessFlags = 0;
	bindBufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bindBufDesc.StructureByteStride = sizeof(Matrix);
	bindBufDesc.ByteWidth = scene->Nodes.size() * sizeof(Matrix);

	D3D11_SUBRESOURCE_DATA data;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	data.pSysMem = dest.data();

	game->Device->CreateBuffer(&bindBufDesc, &data, &AnimFrameBuf);
	game->Device->CreateShaderResourceView(AnimFrameBuf, nullptr, &AnimFrame);
}


void StaticMesh::SetAnimTime(float timeSec)
{
	if (scene == nullptr) return;

	auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(scene->StartTime);
	float dur = std::chrono::duration_cast<std::chrono::milliseconds>(scene->EndTime - scene->StartTime).count() / 1000.0f;

	
	while (timeSec > dur)
		timeSec -= dur;


	std::vector<Matrix> source;
	source.resize(scene->Nodes.size());
	std::vector<Matrix> dest;
	dest.resize(scene->Nodes.size());

	float timePerFrame = dur / (scene->lastFrame - scene->firstFrame);
	int frame = timeSec / timePerFrame;

	if (frame >= scene->lastFrame - scene->firstFrame)
		return;

	scene->GetAnimSnapshot(frame, source);
	source[2].Translation(Vector3(0, source[2].Translation().y, 0));
	scene->ComputeBoneTransforms(source, dest);


	game->Context->UpdateSubresource(AnimFrameBuf, 0, nullptr, dest.data(), 0, 0);
}


StaticMesh::~StaticMesh()
{
}

