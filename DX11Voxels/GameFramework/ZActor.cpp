#include "ZActor.h"
#include "StaticMesh.h"
#include "FbxLoaderExt.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


std::shared_ptr<ZActor> CreateNewStaticMeshActor(const std::string& meshPath)
{
	auto ret = std::make_shared<ZActor>();
	auto meshComp = std::make_shared<ZStaticMeshComponent>();

	auto meshes = LoadAllStaticMeshes(meshPath);

	ret->Components.push_back(meshComp);

	for(auto mesh : meshes)
		meshComp->Meshes.push_back(std::shared_ptr<StaticMesh>(mesh));

	ret->RootComponent = meshComp;

	meshComp->Owner = ret;
	
	return ret;
}


std::shared_ptr<StaticMesh> ProcessStaticMesh(aiNode* node, aiMesh* mesh, const aiScene* scene)
{
	std::vector<VertexPositionNormalBinormalTangentColorTex> vertices(mesh->mNumVertices);
	std::vector<unsigned int> indeces(mesh->mNumFaces*3);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		auto pos	= mesh->mVertices[i];
		auto norm	= mesh->mNormals[i];
		auto biNorm	= mesh->mBitangents[i];
		auto tang	= mesh->mTangents[i];
		
		auto color = aiColor4D(0.8f, 0.8f, 0.8f, 1.0f); 
		if(mesh->mColors) mesh->mColors[0][i];
		auto tex = aiVector3D();
		if(mesh->mTextureCoords) tex = mesh->mTextureCoords[0][i]; // only from first channel


		VertexPositionNormalBinormalTangentColorTex vertex;
		vertex.Position	= Vector4(pos.x, pos.y, pos.z, 1.0f);
		vertex.Normal	= Vector4(norm.x, norm.y, norm.z, 0.0f);
		vertex.Binormal	= Vector4(biNorm.x, biNorm.y, biNorm.z, 0.0f);
		vertex.Tangent	= Vector4(tang.x, tang.y, tang.z, 0.0f);
		vertex.Color	= Vector4(color.r, color.g, color.b, color.a);
		vertex.Tex		= Vector4(tex.x, tex.y, 0.0f, 0.0f);

		vertices[i] = vertex;
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		if (face.mNumIndices > 3) {
			std::cout << "We dont support non-triangle faces << " << __FILE__ << " " << __LINE__ << "\n";
		}

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indeces[i*3 + j] = face.mIndices[j];
	}

	int verticesCount	= vertices.size();
	int indecesCount	= indeces.size();
	int stride = VertexPositionNormalBinormalTangentColorTex::Stride;

	ID3D11Buffer* vertexBuffer, *indexBuffer;

	D3D11_BUFFER_DESC dataBufDesc = {};
	dataBufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	dataBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dataBufDesc.CPUAccessFlags = 0;
	dataBufDesc.MiscFlags = 0;
	dataBufDesc.StructureByteStride = 0;
	dataBufDesc.ByteWidth = verticesCount * stride;

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = &vertices[0];
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;
	Game::Instance->Device->CreateBuffer(&dataBufDesc, &initialData, &vertexBuffer);

	dataBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dataBufDesc.ByteWidth = 4 * indecesCount;
	initialData.pSysMem = &indeces[0];
	Game::Instance->Device->CreateBuffer(&dataBufDesc, &initialData, &indexBuffer);

	auto staticMesh = new StaticMesh(Game::Instance, node->mName.C_Str(), vertexBuffer, indexBuffer, verticesCount, indecesCount, stride);
	staticMesh->Subsets.push_back(ZMeshSubset{.StartPrimitive = 0, .PrimitiveCount = indecesCount/3, .MaterialIndex = 0});

	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		ZMaterial zmat;

		auto mat = scene->mMaterials[mesh->mMaterialIndex];
		auto diffCount = mat->GetTextureCount(aiTextureType_DIFFUSE);
		auto normCount = mat->GetTextureCount(aiTextureType_NORMALS);
		auto rougCount = mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
		auto metaCount = mat->GetTextureCount(aiTextureType_METALNESS);

		aiString diffName, normName, rougName, metaName;

		auto diffRet = mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffName);
		auto normRet = mat->GetTexture(aiTextureType_NORMALS, 0, &normName);
		auto rougRet = mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &rougName);
		auto metaRet = mat->GetTexture(aiTextureType_METALNESS, 0, &metaName);

		zmat.ReloadTextures(diffName.C_Str(), normName.C_Str(), rougName.C_Str(), metaName.C_Str());
		zmat.RoughnessMult = 1.0f;
		zmat.MetallicMult = 1.0f;

		staticMesh->Materials.push_back(zmat);
	}

	return std::shared_ptr<StaticMesh>(staticMesh);
}


std::shared_ptr<ZComponent> ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<ZComponent> parentNode, std::vector<std::shared_ptr<ZComponent>>& components)
{
	auto trans = node->mTransformation;
	aiVector3D pos, scale;
	aiQuaternion quat;
	trans.Decompose(scale, quat, pos);

	ZTransform zTrans;
	zTrans.Position = Vector3(&pos.x);
	zTrans.Rotation = Quaternion(quat.x, quat.y, quat.z, quat.w);
	zTrans.Scale = Vector3(&scale.x);

	std::shared_ptr<ZComponent> zNode;
	ZStaticMeshComponent* meshComp = nullptr;
	ZComponent* zComp = nullptr;

	if (node->mNumMeshes > 0) {
		meshComp = new ZStaticMeshComponent();
		meshComp->Transform = zTrans;

		zNode = std::shared_ptr<ZComponent>(meshComp);
	}
	else {
		zComp = new ZComponent();
		zComp->Transform = zTrans;

		zNode = std::shared_ptr<ZComponent>(zComp);
	}
	components.push_back(zNode);

	zNode->Parent = parentNode;


	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshComp->Meshes.push_back(ProcessStaticMesh(node, mesh, scene));
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene, zNode, components);
	}

	return zNode;
}


std::shared_ptr<ZActor> GAMEFRAMEWORK_API CreateNewStaticMeshActorAssimp(const std::string& meshPath)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(meshPath,
		aiProcess_CalcTangentSpace |
		aiProcess_FlipWindingOrder |
		aiProcess_FlipUVs |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (scene == nullptr) {
		std::cerr << (importer.GetErrorString()) << "\n";
		return std::shared_ptr<ZActor>();
	}

	bool hasAnim = scene->HasAnimations();

	ZActor* actor = new ZActor();

	ProcessNode(scene->mRootNode, scene, nullptr, actor->Components);
	actor->RootComponent = actor->Components[0];

	return std::shared_ptr<ZActor>(actor);
}


ZStaticMeshComponent::ZStaticMeshComponent() : Meshes(0), ZComponent()
{

}


inline void ZStaticMeshComponent::Update(float dt) 
{ 
	//Transform.Update();

	for (auto& mesh : Meshes) {
		mesh->Transform = mesh->LocalTransform * Transform.Transform;
	}

	for (auto& mesh : Meshes)
	{
		if (!mesh->IsSkinned) continue;

		mesh->SetAnimTime(static_cast<float>(Game::Instance->TotalTime));
	}
}


ZStaticMeshComponent::~ZStaticMeshComponent()
{
	std::cout << "~ZStaticMeshComponent\n";
}


ZComponent::~ZComponent()
{
	std::cout << "~ZComponent\n";
}


void ZActor::Update(float dt)
{
	for (auto& comp : Components) {

		comp->Transform.Update();
		if (comp->Parent) {
			comp->Transform.Transform = comp->Parent->Transform.Transform * comp->Transform.Transform;
		}

		if (comp->shouldUpdate) {
			comp->Update(dt);
		}
	}
}


ZActor::~ZActor()
{
	std::cout << "~ZActor\n";
}
