#include "pch.h"
#include "FbxHandler.h"
#include <map>
#include <iostream>
#include "Node.h"
#include "Mesh.h"
#include "MaterialRef.h"


using namespace DirectX::SimpleMath;


Matrix FbxAMatrix2Matrix(FbxAMatrix& matrix)
{
	double* m = matrix;
	return Matrix((float)m[0], (float)m[1], (float)m[2], (float)m[3],
		(float)m[4], (float)m[5], (float)m[6], (float)m[7],
		(float)m[8], (float)m[9], (float)m[10], (float)m[11],
		(float)m[12], (float)m[13], (float)m[14], (float)m[15]);
}


Matrix FbxMatrix2Matrix(FbxMatrix& matrix)
{
	double* m = matrix;
	return Matrix((float)m[0], (float)m[1], (float)m[2], (float)m[3],
		(float)m[4], (float)m[5], (float)m[6], (float)m[7],
		(float)m[8], (float)m[9], (float)m[10], (float)m[11],
		(float)m[12], (float)m[13], (float)m[14], (float)m[15]);
}


Vector3	FbxVector4ToPoint(FbxVector4 v)
{
	return Vector3((float)(v.mData[0] / v.mData[3]),
		(float)(v.mData[1] / v.mData[3]),
		(float)(v.mData[2] / v.mData[3]));
}


Vector3	FbxVector4ToVector(FbxVector4 v)
{
	return Vector3((float)v.mData[0],
		(float)v.mData[1],
		(float)v.mData[2]);
}


Color	FbxColorToColor(FbxColor c)
{
	return Color((float)c.mRed, (float)c.mGreen, (float)c.mBlue, (float)c.mAlpha);
}


Vector2	FbxVector2ToVector(FbxVector2 v)
{
	return Vector2((float)v.mData[0],
		(float)v.mData[1]);
}



FbxNative::FbxHandler::FbxHandler()
{
	fbxManager = FbxManager::Create();
	fbxImporter = FbxImporter::Create(fbxManager, "Importer");
	fbxScene = FbxScene::Create(fbxManager, "Scene");
	fbxGConv = new FbxGeometryConverter(fbxManager);
	
}


void FbxNative::FbxHandler::IterateChildren(FbxNode* fbxNode, FbxScene* fbxScene, Scene* scene, int parentIndex, int depth)
{
	auto node = new Node();
	node->Name = fbxNode->GetName();
	node->ParentIndex = parentIndex;
	//	store FbxNode pointer for further use :
	node->Tag = fbxNode;

	scene->Nodes.emplace_back(node);
	const int index = scene->Nodes.size() - 1;

	const auto trackId = index;
	node->TrackIndex = trackId;


	//Console::WriteLine("{0}{1}", gcnew String(' ', depth*2), node->Name);

	bool ImportAnimation = true;
	//	Animate :
	if (ImportAnimation) {
		for (int frame = scene->firstFrame; frame <= scene->lastFrame; frame++) {
			FbxTime time;
			time.SetFrame(frame, this->timeMode);
			auto animKey = FbxAMatrix2Matrix(fbxNode->GetScene()->GetAnimationEvaluator()->GetNodeLocalTransform(fbxNode, time, FbxNode::eSourcePivot, false, true));
			scene->SetAnimKey(frame, trackId, animKey);
		}
	}

	//	Get transform
	FbxAMatrix	transform = fbxNode->GetScene()->GetAnimationEvaluator()->GetNodeLocalTransform(fbxNode);
	//FbxAMatrix	transform	=	fbxNode->EvaluateLocalTransform();
	node->Transform = FbxAMatrix2Matrix(transform);
	node->BindPose = Matrix::Identity;

	GetCustomProperties(node, fbxNode);


	//	Get bind pose :
	int poseCount = fbxScene->GetPoseCount();
	for (int i = 0; i < poseCount; i++) {
		FbxPose* lPose = fbxScene->GetPose(i);
		if (lPose) {
			//pNode should be the FBX node of bones.
			int lNodeIndex = lPose->Find(fbxNode);
			if (lNodeIndex > -1) {
				// The bind pose is always a global matrix.
				if (lPose->IsBindPose() || !lPose->IsLocalMatrix(lNodeIndex)) {
					FbxMatrix lPoseMatrix = lPose->GetMatrix(lNodeIndex);
					node->BindPose = FbxMatrix2Matrix(lPoseMatrix);
				}
			}
		}
	}


	//	Iterate children :
	for (int i = 0; i < fbxNode->GetChildCount(); i++) {
		FbxNode* fbxChild = fbxNode->GetChild(i);
		IterateChildren(fbxChild, fbxScene, scene, index, depth + 1);
	}
}
//*/

void FbxNative::FbxHandler::GetCustomProperties(Node* node, FbxNode* fbxNode)
{
	auto lProperty = fbxNode->GetFirstProperty();
	while (lProperty.IsValid()) {
		if (lProperty.GetFlag(FbxPropertyFlags::eUserDefined)) {
			std::string pName = lProperty.GetName().Buffer();
			//Console::Write("Custom property name: ");
			//Console::WriteLine(pName);

			void* custProp;
			EFbxType pDataType = lProperty.GetPropertyDataType().GetType();

			if (pDataType == eFbxBool) {
				FbxBool propBool = lProperty.Get<FbxBool>();
				custProp = new bool(propBool);
			}
			else if (pDataType == eFbxFloat || pDataType == eFbxDouble) {
				FbxDouble propDouble = lProperty.Get<FbxDouble>();
				custProp = new float((float)propDouble);
			}
			else if (pDataType == eFbxInt) {
				FbxInt propInt = lProperty.Get<FbxInt>();
				custProp = new int(propInt);
			}
			else if (pDataType == eFbxString) {
				FbxString propString = lProperty.Get<FbxString>();
				custProp = new std::string(propString.Buffer());
			}
			else if (pDataType == eFbxDouble3 || pDataType == eFbxDouble4) {
				FbxDouble3 propVector = lProperty.Get<FbxDouble3>();
				custProp = new Vector3((float)propVector[0], (float)propVector[1], (float)propVector[2]);
			}

			//node->Attributes->Add(pName, custProp);
		}

		lProperty = fbxNode->GetNextProperty(lProperty);
	}
}



/*-----------------------------------------------------------------------------
Mesh stuff :
-----------------------------------------------------------------------------*/
#pragma region Mesh material helpers

std::string GetTextureFileName(FbxTexture* pTexture)
{
	FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(pTexture);

	if (lFileTexture) {
		return lFileTexture->GetRelativeFileName();
	}
	else {
		return "";
	}
}


void TryGetDiffuseTexture(std::string textureName, FbxProperty pProperty, int pMaterialIndex)
{
	if (pProperty.IsValid()) {

		std::string propertyName = pProperty.GetNameAsCStr();

		if (propertyName == "DiffuseColor") {

			int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();

			if (lTextureCount <= 0) {
				return;
			}

			FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(0);

			if (lTexture) {
				textureName = GetTextureFileName(lTexture);
			}
		}
	}
}


/*
**	RetriveTextureDictionaryByProperty
*/
void AddTextureToDictionaryByProperty(std::map<std::string, std::string>* dict, FbxProperty pProperty, int pMaterialIndex)
{
	if (pProperty.IsValid()) {

		std::string propertyName = pProperty.GetNameAsCStr();
		//Console::WriteLine(propertyName);

		//return;

		int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();

		for (int j = 0; j < lTextureCount; ++j) {

			//Here we have to check if it's layered textures, or just textures:
			FbxLayeredTexture* lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);

			if (lLayeredTexture) {

				printf("%s : Layered textures are not supported", propertyName.c_str());
				throw std::exception();

				//FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
				//int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
				//
				//for ( int k =0; k<lNbTextures; ++k ) {
				//	FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
				//	if ( lTexture ) {
				//		string ^fileName = GetTextureFileName( lTexture);   
				//	}
				//}

			}
			else {
				//no layered texture simply get on the property
				FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
				if (lTexture) {
					auto fileName = GetTextureFileName(lTexture);

					dict->emplace(propertyName, fileName);
				}
			}
		}
	}
}

//#pragma warning (disable: 4996)

#pragma endregion //Mesh material helpers


/*
**	FbxLoader::HandleMesh
*/
void FbxNative::FbxHandler::HandleMesh(Scene* scene, Node* node, FbxNode* fbxNode)
{
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//return;

	if (!fbxMesh) {
		return;
	}

	Mesh* nodeMesh = new Mesh();

	scene->Meshes.emplace_back(nodeMesh);
	node->MeshIndex = scene->Meshes.size() - 1;

	FbxMesh* temp = nullptr;
	if (!fbxMesh->IsTriangleMesh()) {
		temp = (FbxMesh*)fbxGConv->Triangulate(fbxMesh, false, false);
	}
	if (temp != nullptr) fbxMesh = temp;
	fbxMesh->GenerateTangentsDataForAllUVSets();
	Matrix meshTransform = Matrix::Identity;
	std::vector <Int4>	skinIndices = std::vector<Int4>(fbxMesh->GetControlPointsCount());
	std::vector<Vector4> skinWeights = std::vector<Vector4>(fbxMesh->GetControlPointsCount());

	HandleSkinning(nodeMesh, scene, node, fbxNode, &meshTransform, skinIndices, skinWeights);

	int polyCount = fbxMesh->GetPolygonCount();

	int vertexIdCount = 0;

	nodeMesh->Vertices.reserve(polyCount * 3);

	//
	//	vertices :
	//	
	for (int i = 0; i < polyCount; i++) {

		int n = fbxMesh->GetPolygonSize(i);
		if (n != 3) {
			std::cout << "Bad triangle, ignored" << std::endl;;
			continue;
		}

		MeshTriangle tri;

		for (int j = 0; j < 3; j++) {
			int			id = fbxMesh->GetPolygonVertex(i, j);
			FbxVector4	p = fbxMesh->GetControlPointAt(id);

			MeshVertex v;

			Vector3 transfPos = Vector3::Transform(FbxVector4ToVector(p), meshTransform);

			v.Color0 = Color(1.0f, 1.0f, 1.0f, 1.0f);
			v.Position = Vector3(transfPos.x, transfPos.y, transfPos.z);

			GetNormalForVertex(&v, fbxMesh, vertexIdCount, id);
			GetTextureForVertex(&v, fbxMesh, vertexIdCount, id);
			GetColorForVertex(&v, fbxMesh, vertexIdCount, id);
			GetTangentForVertex(&v, fbxMesh, vertexIdCount, id);
			GetBinormalForVertex(&v, fbxMesh, vertexIdCount, id);
			
			v.SkinIndices = skinIndices[id];
			v.SkinWeights = skinWeights[id];

			//Log::Message("{0,5} {1,5} {2,5} {3,5} : {4,5:0.000} {5,5:0.000} {6,5:0.000} {7,5:0.000}", 
			//	v.SkinIndices.X, v.SkinIndices.Y, v.SkinIndices.Z, v.SkinIndices.W, 
			//	v.SkinWeights.X, v.SkinWeights.Y, v.SkinWeights.Z, v.SkinWeights.W );

			v.TexCoord0.y = 1 - v.TexCoord0.y;

			nodeMesh->Vertices.emplace_back(v);

			if (j == 0) tri.Index0 = vertexIdCount;
			if (j == 1) tri.Index1 = vertexIdCount;
			if (j == 2) tri.Index2 = vertexIdCount;

			vertexIdCount++;
		}

		nodeMesh->Triangles.emplace_back(tri);
	}

	//	
	//	materials :
	//
	int mtrlCount = fbxNode->GetMaterialCount();

	auto mtrlMap = std::map<int, int>();

	for (int i = 0; i < mtrlCount; i++) {

		MaterialRef* mtrl = new MaterialRef();
		FbxSurfaceMaterial* fbxMtrl = fbxNode->GetMaterial(i);

		if (fbxMtrl) {
			FbxProperty _property;
			int textureIndex;

			mtrl->Name = std::string(fbxMtrl->GetName());

			FbxProperty lProperty;
			//std::string texturePath = nullptr;

			//	retrive texture names :
			FBXSDK_FOR_EACH_TEXTURE(textureIndex) {
				auto texChannelName = FbxLayerElement::sTextureChannelNames[textureIndex];
				_property = fbxMtrl->FindProperty(texChannelName);

				if (strcmp(texChannelName, reinterpret_cast<const char*>(u8"DiffuseColor")) == 0) {
					FbxTexture* lTexture = _property.GetSrcObject<FbxTexture>(0);

					if (lTexture) {
						mtrl->DiffuseMap = GetTextureFileName(lTexture);
					}
				}
				else if (strcmp(texChannelName, reinterpret_cast<const char*>(u8"Bump")) == 0) {
					FbxTexture* lTexture = _property.GetSrcObject<FbxTexture>(0);

					if (lTexture) {
						mtrl->NormalMap = GetTextureFileName(lTexture);
					}
				}
				else if (strcmp(texChannelName, reinterpret_cast<const char*>(u8"SpecularFactor")) == 0) {
					FbxTexture* lTexture = _property.GetSrcObject<FbxTexture>(0);

					if (lTexture) {
						mtrl->SpecularMap = GetTextureFileName(lTexture);
					}
				}

			}
		}


		auto itr = std::find(scene->Materials.begin(), scene->Materials.end(), mtrl);

		if (itr != scene->Materials.end()) {
			mtrlMap[i] = std::distance(scene->Materials.begin(), itr);
		}
		else {
			scene->Materials.emplace_back(mtrl);
			mtrlMap[i] = scene->Materials.size() - 1;
		}
	}

	//
	//	Retrive material mapping :
	//	
	const int lPolygonCount = fbxMesh->GetPolygonCount();

	// Count the polygon count of each material
	FbxLayerElementArrayTemplate<int>* lMaterialIndices = NULL;
	FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;

	if (fbxMesh->GetElementMaterial())
	{
		lMaterialIndices = &fbxMesh->GetElementMaterial()->GetIndexArray();
		lMaterialMappingMode = fbxMesh->GetElementMaterial()->GetMappingMode();

		if (!lMaterialIndices) {
			throw std::exception(); //gcnew Exception(String::Format("No material indices"));
		}

		if (lMaterialMappingMode == FbxGeometryElement::eByPolygon) {

			FBX_ASSERT(lMaterialIndices->GetCount() == lPolygonCount);

			for (int i = 0; i < lPolygonCount; i++) {
				int i0 = nodeMesh->Triangles[i].Index0;
				int i1 = nodeMesh->Triangles[i].Index1;
				int i2 = nodeMesh->Triangles[i].Index2;
				nodeMesh->Triangles[i] = MeshTriangle(i0, i1, i2, mtrlMap[lMaterialIndices->GetAt(i)]);
			}

		}
		else if (lMaterialMappingMode == FbxGeometryElement::eAllSame) {

			for (int i = 0; i < lPolygonCount; i++) {
				int i0 = nodeMesh->Triangles[i].Index0;
				int i1 = nodeMesh->Triangles[i].Index1;
				int i2 = nodeMesh->Triangles[i].Index2;
				nodeMesh->Triangles[i] = MeshTriangle(i0, i1, i2, mtrlMap[lMaterialIndices->GetAt(0)]);
			}


			nodeMesh->Subsets.push_back(MeshSubset{
				0,
				lPolygonCount,
				nodeMesh->Triangles[0].MaterialIndex
			});
		}
		else {
			throw std::exception();// String::Format("Unsupported mapping mode"));
		}

	}
	else {
		for (int i = 0; i < lPolygonCount; i++) {
			int i0 = nodeMesh->Triangles[i].Index0;
			int i1 = nodeMesh->Triangles[i].Index1;
			int i2 = nodeMesh->Triangles[i].Index2;
			nodeMesh->Triangles[i] = MeshTriangle(i0, i1, i2, -1);
		}
		nodeMesh->Subsets.push_back(MeshSubset{
				0,
				lPolygonCount,
				nodeMesh->Triangles[0].MaterialIndex
		});
	}

	if (temp) temp->Destroy();
}



void FbxNative::FbxHandler::HandleMaterial(MeshSubset* sg, FbxSurfaceMaterial* material)
{

}


int GetFbxNodeIndex(FbxNative::Scene* scene, FbxNode* fbxNode)
{
	for (int i = 0; i < scene->Nodes.size(); i++) {
		if (scene->Nodes[i]->Tag == fbxNode) {
			return i;
		}
	}
	return -1;
}


/*-------------------------------------------------------------------------------------------------

Animation stuff :

-------------------------------------------------------------------------------------------------*/

/*
**	Fusion::Fbx::FbxLoader::HandleAnimation
*/
void FbxNative::FbxHandler::HandleSkinning(Mesh* nodeMesh, FbxNative::Scene* scene, FbxNative::Node* node, FbxNode* fbxNode, Matrix* meshTransform, std::vector<Int4>& skinIndices, std::vector<Vector4>& skinWeights)
{
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	int clusterCount = 0;

	for (int i = 0; i < fbxMesh->GetDeformerCount(FbxDeformer::eSkin); i++) {
		clusterCount += ((FbxSkin*)(fbxMesh->GetDeformer(i, FbxDeformer::eSkin)))->GetClusterCount();
	}


	if (clusterCount > 0) {
		FbxSkin* skinDeformer = (FbxSkin*)fbxMesh->GetDeformer(0, FbxDeformer::eSkin);
		FbxSkin::EType	skinningType = skinDeformer->GetSkinningType();

		if (skinningType == FbxSkin::eLinear || skinningType == FbxSkin::eRigid || skinningType == FbxSkin::eDualQuaternion) {
			nodeMesh->isSkinned = true;

			// Control points
			int* vertexWeightsCounter = new int[fbxMesh->GetControlPointsCount()]();

			int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin); // Count of skeletons used for skinning

			for (int defInd = 0; defInd < deformerCount; defInd++) {
				skinDeformer = (FbxSkin*)fbxMesh->GetDeformer(defInd, FbxDeformer::eSkin);
				int clusterCount = skinDeformer->GetClusterCount();							// Count of bones in skeleton

				for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex) {

					FbxCluster* cluster = skinDeformer->GetCluster(clusterIndex);

					if (!cluster->GetLink()) {
						std::cout << "Missing link" << std::endl;
					}

					FbxNode* fbxLinkNode = cluster->GetLink();

					int vertexIndexCount = cluster->GetControlPointIndicesCount();


					for (int k = 0; k < vertexIndexCount; ++k) {

						int index = cluster->GetControlPointIndices()[k];

						// Sometimes, the mesh can have less points than at the time of the skinning
						// because a smooth operator was active when skinning but has been deactivated during export.
						if (index >= fbxMesh->GetControlPointsCount()) {
							std::cout << "The mesh had less points than at the time of the skinning." << std::endl;
							continue;
						}

						double weight = cluster->GetControlPointWeights()[k];

						auto vWeightCount = vertexWeightsCounter[index];
						if (vWeightCount >= 4) {
							//Console::WriteLine("Vertex has more than 4 influences: " + vertexWeightsCounter[index].ToString());
							continue;
						}

						// PVS-Studio : 
						// V550 An odd precise comparison: weight == 0.0. 
						// It's probably better to use a comparison with defined precision: 
						//	fabs(A - B) < Epsilon. fbxloader.cpp 484
						if (weight == 0.0) {
							continue;
						}

						auto& localInd = vertexWeightsCounter[index];
						auto& skinIndex = skinIndices[index];
						skinIndex[localInd] = GetFbxNodeIndex(scene, fbxLinkNode);

						auto& skinWeight = skinWeights[index];
						skinWeight[localInd] = (float)weight;

						localInd++;
					}
				}

			}

			delete[] vertexWeightsCounter;
		}
		else {
			//System::Console::WriteLine(gcnew String("Unsupported skinning type deformation"));
		}
	}
}


/*-----------------------------------------------------------------------------
Vertex attribute stuff :
-----------------------------------------------------------------------------*/


void FbxNative::FbxHandler::GetColorForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int ctrlPointId)
{
	FbxGeometryElementVertexColor* colorElement = fbxMesh->GetElementVertexColor();

	if (!colorElement) {
		return;
	}

	auto mapMode = colorElement->GetMappingMode();
	auto refMode = colorElement->GetReferenceMode();

	if (mapMode == FbxGeometryElement::eDirect) {

		int colorIndex = 0;

		if (refMode == FbxGeometryElement::eDirect) {
			colorIndex = ctrlPointId;
		}
		if (refMode == FbxGeometryElement::eIndexToDirect) {
			colorIndex = colorElement->GetIndexArray().GetAt(ctrlPointId);
		}

		vertex->Color0 = FbxColorToColor(colorElement->GetDirectArray().GetAt(colorIndex));
	}
	else if (mapMode == FbxGeometryElement::eByPolygonVertex) {

		int colorIndex = 0;

		if (refMode == FbxGeometryElement::eDirect) {
			colorIndex = vertexIdCount;
		}

		if (refMode == FbxGeometryElement::eIndexToDirect) {
			colorIndex = colorElement->GetIndexArray().GetAt(vertexIdCount);
		}

		vertex->Color0 = FbxColorToColor(colorElement->GetDirectArray().GetAt(colorIndex));

	}
	else {
		vertex->Color0 = Color(1.0f, 1.0f, 1.0f, 1.0f);
		//throw gcnew Exception(gcnew String("Unsupported color mapping mode"));
	}

}


/*
**	Fusion::Fbx::FbxLoader::GetNormalForVertex
*/
void FbxNative::FbxHandler::GetNormalForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int ctrlPointId)
{
	FbxGeometryElementNormal* normalElement = fbxMesh->GetElementNormal();

	if (normalElement) {
		if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {

			int vertCount = fbxMesh->GetControlPointsCount();

			int normalIndex = 0;

			if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
				normalIndex = ctrlPointId;
			}

			if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
				normalIndex = normalElement->GetIndexArray().GetAt(ctrlPointId);
			}

			FbxVector4 normalVector = normalElement->GetDirectArray().GetAt(normalIndex);

			vertex->Normal = FbxVector4ToVector(normalVector);
		}

		else if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {

			int normalIndex = 0;

			if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
				normalIndex = vertexIdCount;
			}

			if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
				normalIndex = normalElement->GetIndexArray().GetAt(vertexIdCount);
			}

			FbxVector4 normalVector = normalElement->GetDirectArray().GetAt(normalIndex);
			vertex->Normal = FbxVector4ToVector(normalVector);
		}
	}
}


void FbxNative::FbxHandler::GetTangentForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int ctrlPointId)
{
	FbxGeometryElementTangent* tangentElement = fbxMesh->GetElementTangent();

	if (tangentElement) {
		if (tangentElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {

			int vertCount = fbxMesh->GetControlPointsCount();

			int tangentIndex = 0;

			if (tangentElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
				tangentIndex = ctrlPointId;
			}

			if (tangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
				tangentIndex = tangentElement->GetIndexArray().GetAt(ctrlPointId);
			}

			FbxVector4 tangentVector = tangentElement->GetDirectArray().GetAt(tangentIndex);

			vertex->Tangent = FbxVector4ToVector(tangentVector);
		}

		else if (tangentElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {

			int tangentIndex = 0;

			if (tangentElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
				tangentIndex = vertexIdCount;
			}

			if (tangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
				tangentIndex = tangentElement->GetIndexArray().GetAt(vertexIdCount);
			}

			FbxVector4 normalVector = tangentElement->GetDirectArray().GetAt(tangentIndex);
			vertex->Tangent = FbxVector4ToVector(normalVector);
		}
	}
}



void FbxNative::FbxHandler::GetBinormalForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int ctrlPointId)
{
	FbxGeometryElementBinormal* binormalElement = fbxMesh->GetElementBinormal();

	if (binormalElement) {
		if (binormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {

			int vertCount = fbxMesh->GetControlPointsCount();

			int binormalIndex = 0;

			if (binormalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
				binormalIndex = ctrlPointId;
			}

			if (binormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
				binormalIndex = binormalElement->GetIndexArray().GetAt(ctrlPointId);
			}

			FbxVector4 binormalVector = binormalElement->GetDirectArray().GetAt(binormalIndex);

			vertex->Binormal = FbxVector4ToVector(binormalVector);
		}

		else if (binormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {

			int binormalIndex = 0;

			if (binormalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
				binormalIndex = vertexIdCount;
			}

			if (binormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
				binormalIndex = binormalElement->GetIndexArray().GetAt(vertexIdCount);
			}

			FbxVector4 binormalVector = binormalElement->GetDirectArray().GetAt(binormalIndex);
			vertex->Binormal = FbxVector4ToVector(binormalVector);
		}
	}
}



/*
**	Fusion::Fbx::FbxLoader::GetTextureForVertex
*/
void FbxNative::FbxHandler::GetTextureForVertex(MeshVertex* vertex, FbxMesh* fbxMesh, int vertexIdCount, int vertexId)
{
	FbxStringList NameListOfUV;
	fbxMesh->GetUVSetNames(NameListOfUV);

	for (int setIndexOfUV = 0; setIndexOfUV < NameListOfUV.GetCount(); setIndexOfUV++) {

		const char* nameOfUVSet = NameListOfUV.GetStringAt(setIndexOfUV);
		const FbxGeometryElementUV* elementOfUV = fbxMesh->GetElementUV(nameOfUVSet);

		if (!elementOfUV) {
			continue;
		}

		if (elementOfUV->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			elementOfUV->GetMappingMode() != FbxGeometryElement::eByControlPoint) {
			return;
		}

		const bool useIndex = elementOfUV->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int indexOfUVCount = (useIndex) ? elementOfUV->GetIndexArray().GetCount() : 0;


		if (elementOfUV->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
			FbxVector2 valueOfUV;

			int indexOfUV = useIndex ? elementOfUV->GetIndexArray().GetAt(vertexId) : vertexId;

			valueOfUV = elementOfUV->GetDirectArray().GetAt(indexOfUV);

			switch (setIndexOfUV) {
			case 0: { vertex->TexCoord0 = FbxVector2ToVector(valueOfUV); }
				  //case 1 : { vertex->TexCoord1 = FbxVector2ToVector(valueOfUV); }
			}
		}
		else if (elementOfUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			if (vertexIdCount < indexOfUVCount) {
				FbxVector2 valueOfUV;
				int indexOfUV = useIndex ? elementOfUV->GetIndexArray().GetAt(vertexIdCount) : vertexIdCount;

				valueOfUV = elementOfUV->GetDirectArray().GetAt(indexOfUV);

				switch (setIndexOfUV) {
				case 0: {vertex->TexCoord0 = FbxVector2ToVector(valueOfUV); }
					  //case 1 : {vertex->TexCoord1 = FbxVector2ToVector(valueOfUV); }
				}

			}
		}
	}
}



/*
**	Fusion::Fbx::FbxLoader::HandleNull
*/
void FbxNative::FbxHandler::HandleCamera(Scene* scene, Node* node, FbxNode* fbxNode)
{

}


/*
**	Fusion::Fbx::FbxLoader::HandleLight
*/
void FbxNative::FbxHandler::HandleLight(Scene* scene, Node* node, FbxNode* fbxNode)
{

}



