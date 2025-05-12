#pragma once
#include <string>

#ifdef FBXLOADER_EXPORTS  
#define FBXLOADER_API __declspec(dllexport)   
#else  
#define FBXLOADER_API __declspec(dllimport)   
#endif

namespace FbxNative {

	class FBXLOADER_API MaterialRef
	{
	public:
		std::string Name;

		std::string DiffuseMap;
		std::string NormalMap;
		std::string SpecularMap;
		std::string RoughnessMap;
		std::string MetallicMap;
	};

}
