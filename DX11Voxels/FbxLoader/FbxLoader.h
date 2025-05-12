#pragma once

#ifdef FBXLOADER_EXPORTS  
#define FBXLOADER_API __declspec(dllexport)   
#else  
#define FBXLOADER_API __declspec(dllimport)   
#endif



//#define FBXSDK_NEW_API

#include "Scene.h"
#include <string>


namespace FbxNative {

	class FbxHandler;

	class FBXLOADER_API FbxLoader
	{
		
		FbxHandler* handler;

		bool ImportAnimation = true;
		bool ImportGeometry = true;

	public:
		FbxLoader();


		Scene* LoadScene(const std::string& filePath);

	};

}
