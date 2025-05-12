#include "d3d11.h"
#include "FbxLoaderExt.h"

#include "VertexSkin.h"


void LoadSubmeshesWithMaterials(const std::string& fileName, const FbxNative::Mesh* fbxMesh, const FbxNative::Scene* scene, StaticMesh* mesh)
{
	if(fbxMesh->Subsets.size() == 0) {
		auto zSubset = ZMeshSubset{
			0,
			fbxMesh->GetTriangleCount(),
			0
		};

		auto material = ZMaterial();
		material.RoughnessMult = 1.0f;
		material.ReloadTextures(
			L"./Content/DefaultDiffuseMap.jpg",
			L"./Content/DefaultNormalMap.jpg",
			L"./Content/DefaultRoughnessMap.jpg",
			L"./Content/DefaultMetallicMap.jpg");

		mesh->Materials.emplace_back(material);
		mesh->Subsets.emplace_back(zSubset);

		return;
	}

	for (auto& subset : fbxMesh->Subsets) {
		auto zSubset = ZMeshSubset{
			subset.StartPrimitive,
			subset.PrimitiveCount,
			(int)mesh->Materials.size()
		};

		auto material = ZMaterial();
		material.RoughnessMult = 1.0f;

		auto matIndex = subset.MaterialIndex;
		if (matIndex < 0) {
			material.ReloadTextures(
				L"./Content/DefaultDiffuseMap.jpg", 
				L"./Content/DefaultNormalMap.jpg", 
				L"./Content/DefaultRoughnessMap.jpg",
				L"./Content/DefaultMetallicMap.jpg");
		}
		else {
			auto path = std::filesystem::path(fileName).parent_path().string();
			auto mat = scene->Materials[matIndex];

			auto difPath	= path + "/" + mat->DiffuseMap;
			auto norPath	= path + "/" + mat->NormalMap;
			auto roughPath	= path + "/" + mat->RoughnessMap;
			auto metPath	= path + "/" + mat->MetallicMap;

			if (mat->DiffuseMap.empty())	difPath = "./Content/DefaultDiffuseMap.jpg";
			if (mat->NormalMap.empty())		norPath = "./Content/DefaultNormalMap.jpg";
			if (mat->RoughnessMap.empty())	roughPath = "./Content/DefaultRoughnessMap.jpg";
			if (mat->MetallicMap.empty())	metPath = "./Content/DefaultMetallicMap.jpg";

			std::wstring difPathW = std::wstring(difPath.begin(), difPath.end());
			std::wstring norPathW = std::wstring(norPath.begin(), norPath.end());
			std::wstring rouPathW = std::wstring(roughPath.begin(), roughPath.end());
			std::wstring metPathW = std::wstring(metPath.begin(), metPath.end());

			material.ReloadTextures(difPathW, norPathW, rouPathW, metPathW);
		}

		mesh->Materials.emplace_back(material);
		mesh->Subsets.emplace_back(zSubset);
	}
}


template<typename T>
StaticMesh* LoadMesh(FbxNative::Mesh* fbxMesh, FbxNative::Scene* scene, const std::string& fileName)
{
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	std::vector<T> verts;
	std::vector<int> inds;

	ConvertStaticMeshToVertices<T>(fbxMesh, verts, inds);

	int verticesCount = verts.size();
	int indecesCount = inds.size();

	D3D11_BUFFER_DESC dataBufDesc = {};
	dataBufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	dataBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dataBufDesc.CPUAccessFlags = 0;
	dataBufDesc.MiscFlags = 0;
	dataBufDesc.StructureByteStride = 0;
	dataBufDesc.ByteWidth = verticesCount * T::Stride;

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = &verts[0];
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;

	Game::Instance->Device->CreateBuffer(&dataBufDesc, &initialData, &vertexBuffer);

	dataBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dataBufDesc.ByteWidth = 4 * indecesCount;
	initialData.pSysMem = &inds[0];

	Game::Instance->Device->CreateBuffer(&dataBufDesc, &initialData, &indexBuffer);

	auto mesh = new StaticMesh(Game::Instance, fileName, vertexBuffer, indexBuffer, verticesCount, indecesCount, T::Stride);

	LoadSubmeshesWithMaterials(fileName, fbxMesh, scene, mesh);

	return mesh;
}


std::vector<StaticMesh*> LoadAllStaticMeshes(const std::string& fileName)
{
	std::vector<StaticMesh*> meshes;

	auto scene = Game::Instance->FbxLoader->LoadScene(fileName);


	for (int i = 0; i < scene->Nodes.size(); i++) {
		auto node = scene->Nodes[i];

		if (node->MeshIndex < 0) {
			continue;
		}

		const auto fbxMesh = scene->Meshes[node->MeshIndex];

		StaticMesh* mesh = nullptr;
		if (fbxMesh->isSkinned) {
			mesh = LoadMesh<VertexSkin>(fbxMesh, scene, fileName);
			mesh->scene = scene;
			mesh->IsSkinned = true;
			mesh->InitSkeletalData();
		} else {
			mesh = LoadMesh<VertexPositionNormalBinormalTangentColorTex>(fbxMesh, scene, fileName);
		}
		meshes.push_back(mesh);
	}

	//delete scene;

	return meshes;
}
