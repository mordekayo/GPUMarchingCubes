#include "pch.h"
#include "FbxLoader.h"
#include <fbxsdk\core\fbxpropertydef.h>
#include <fbxsdk\scene\geometry\fbxnode.h>
#include <fbxsdk\scene\geometry\fbxmesh.h>
#include <fbxsdk/core/fbxproperty.h>
#include <iostream>
#include <map>
#include "FbxHandler.h"
#include "Node.h"

/////////////////////////////////////////////////////////////////////////////////////////////


FbxNative::FbxLoader::FbxLoader()
{
	std::cout << "FBX SDK " << FbxManager::GetVersion() << std::endl;
	
	handler = new FbxHandler();
}


/*-----------------------------------------------------------------------------
FBX loader :
-----------------------------------------------------------------------------*/

FbxNative::Scene* FbxNative::FbxLoader::LoadScene(const std::string& filePath)
{
	std::cout << filePath << std::endl;
	const char* fileName = filePath.c_str();

	if (!handler->fbxImporter->Initialize(fileName)) {
		throw std::exception("Failed to initialize the FBX importer");
	}


	if (!handler->fbxImporter->Import(handler->fbxScene)) {
		throw std::exception("Failed to import the scene");
	}

	
	FbxTimeSpan timeSpan;
	FbxTime		start;
	FbxTime		end;

	auto animStack = handler->fbxScene->GetCurrentAnimationStack();
	if (animStack == nullptr) {
		handler->fbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeSpan);
	}
	else {
		timeSpan = animStack->GetLocalTimeSpan();
	}
	handler->timeMode = handler->fbxScene->GetGlobalSettings().GetTimeMode();
	start	= timeSpan.GetStart();
	end		= timeSpan.GetStop();

	FbxNode *rootNode = handler->fbxScene->GetRootNode();

	Scene *scene = new Scene();

	if (ImportAnimation) {
		scene->StartTime	= std::chrono::time_point<std::chrono::steady_clock>(std::chrono::milliseconds((long)start.GetMilliSeconds()));
		scene->EndTime		= std::chrono::time_point<std::chrono::steady_clock>(std::chrono::milliseconds((long)end.GetMilliSeconds()));
	
		scene->CreateAnimation((int)start.GetFrameCount(handler->timeMode), (int)end.GetFrameCount(handler->timeMode), handler->fbxScene->GetNodeCount());
	}

	std::cout << "Traversing hierarchy..." << std::endl;

	handler->IterateChildren(rootNode, handler->fbxScene, scene, -1, 1);

	std::cout << "Import Geometry..." << std::endl;

	if (ImportGeometry) {
		for(Node *node : scene->Nodes) {
			FbxNode *fbxNode = (FbxNode*)(node->Tag);
			handler->HandleMesh(scene, node, fbxNode);
		}
	}

	//	do not destroy...
	// 	stack overflow happens...
	//fbxImporter->Destroy(true);

	return scene;
}

