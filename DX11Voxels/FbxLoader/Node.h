#pragma once
#include <string>
#include "../ZMathLib/SimpleMath.h"

#ifdef FBXLOADER_EXPORTS  
#define FBXLOADER_API __declspec(dllexport)   
#else  
#define FBXLOADER_API __declspec(dllimport)   
#endif

using namespace DirectX::SimpleMath;


namespace FbxNative {

	class FBXLOADER_API Node
	{
	public:
		std::string Name;

		/// <summary>
		/// Parent index in scene. Zero value means root node.
		/// </summary>
		int ParentIndex;

		/// <summary>
		/// Scene mesh index. Negative value means no mesh reference.
		/// </summary>
		int MeshIndex;

		/// <summary>
		/// Scene animation track index.
		/// Negative value means no animation track.
		/// </summary>
		int TrackIndex;

		/// <summary>
		/// Node transform
		/// </summary>
		Matrix Transform;

		/// <summary>
		/// Global matrix of "bind-posed" node.
		/// For nodes that do not affect skinning this value is always Matrix.Identity.
		/// </summary>
		Matrix BindPose;

		/// <summary>
		/// Tag object. This value will not be serialized.
		/// </summary>
		void* Tag;


		/// <summary>
		/// Creates instance of the node.
		/// </summary>
		Node() {
			MeshIndex = -1;
			ParentIndex = -1;
			TrackIndex = -1;
		}
	};

}
