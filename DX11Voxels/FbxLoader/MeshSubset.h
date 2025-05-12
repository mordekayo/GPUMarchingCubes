#pragma once


#ifdef FBXLOADER_EXPORTS  
#define FBXLOADER_API __declspec(dllexport)   
#else  
#define FBXLOADER_API __declspec(dllimport)   
#endif

namespace FbxNative {

	class FBXLOADER_API MeshSubset
	{
	public:

		int StartPrimitive;

		int PrimitiveCount;

		int MaterialIndex;

	};

}
