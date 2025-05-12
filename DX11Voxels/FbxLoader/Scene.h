#pragma once

#pragma warning (disable: 4251)

#ifdef FBXLOADER_EXPORTS  
#define FBXLOADER_API __declspec(dllexport)   
#else  
#define FBXLOADER_API __declspec(dllimport)   
#endif


#include <vector>
#include <chrono>
#include "Node.h"
#include "../ZMathLib/SimpleMath.h"



namespace FbxNative {
	//class Node;
	class Mesh;
	class MaterialRef;

	
	class FBXLOADER_API Scene {

	public:
		/// <summary>
		/// List of scene nodes
		/// </summary>
		std::vector<Node*> Nodes;

		/// <summary>
		/// List of scene meshes.
		/// </summary>
		std::vector<Mesh*> Meshes;

		std::vector<MaterialRef*> Materials;


		int firstFrame = 0;
		int lastFrame = 0;
		int trackCount = 0;
		DirectX::SimpleMath::Matrix** animData = nullptr;

		
		std::chrono::time_point<std::chrono::steady_clock> StartTime;
		std::chrono::time_point<std::chrono::steady_clock> EndTime;


		void CreateAnimation(int firstFrame, int lastFrame, int nodeCount)
		{
			if (firstFrame > lastFrame) {
				throw new std::exception("firstFrame > lastFrame");
			}
			if (nodeCount <= 0) {
				throw new std::exception("nodeCount must be positive");
			}

			this->firstFrame = firstFrame;
			this->lastFrame = lastFrame;
			this->trackCount = nodeCount;

			auto framesCount = lastFrame - firstFrame + 1;
			animData = new DirectX::SimpleMath::Matrix*[framesCount];
			for (int i = 0; i < framesCount; i++) {
				animData[i] = new DirectX::SimpleMath::Matrix[nodeCount];
			}
		}



		void ComputeAbsoluteTransforms(std::vector<DirectX::SimpleMath::Matrix>& source, std::vector<DirectX::SimpleMath::Matrix>& destination)
		{
			if (source.size() < Nodes.size()) {
				throw "source.Length must be greater of equal to Nodes.Count";
			}

			if (destination.size() < Nodes.size()) {
				throw "destination.Length must be greater of equal to Nodes.Count";
			}

			for (int i = Nodes.size() - 1; i >= 0; i--) {

				auto node = Nodes[i];
				auto transform = source[i];
				auto parentIndex = node->ParentIndex;

				while (parentIndex != -1) {
					transform = transform * source[parentIndex];
					parentIndex = Nodes[parentIndex]->ParentIndex;
				}

				destination[i] = transform;
			}
		}



		/// <summary>
		/// Computes bones transforms for skinning taking in account bind position.
		/// </summary>
		/// <param name="source">Local bone transforms</param>
		/// <param name="destination">Global bone transforms multiplied by bind pose matrix</param>
		void ComputeBoneTransforms(std::vector<DirectX::SimpleMath::Matrix>& source, std::vector<DirectX::SimpleMath::Matrix>& destination)
		{
			ComputeAbsoluteTransforms(source, destination);
			for (int i = 0; i < Nodes.size(); i++) {
				destination[i] = (Nodes[i]->BindPose.Invert() * destination[i]).Transpose();
			}
		}


		void GetAnimSnapshot(int frame, std::vector<DirectX::SimpleMath::Matrix>& destination)
		{
			if (animData == nullptr) {
				throw "Animation data is not created";
			}

			if (destination.size() < Nodes.size()) {
				throw "destination.Length must be greater of equal to Nodes.Count";
			}

			for (int i = 0; i < Nodes.size(); i++) {
				auto node = Nodes[i];
				destination[i] = node->TrackIndex < 0 ? node->Transform : GetAnimKey(frame, node->TrackIndex);
			}
		}


		/// <summary>
		/// Sets animation key
		/// </summary>
		/// <param name="frame"></param>
		/// <param name="nodeIndex"></param>
		/// <param name="transform"></param>
		void SetAnimKey(int frame, int trackIndex, const DirectX::SimpleMath::Matrix& transform)
		{
			if (animData == nullptr) {
				throw new std::exception("Animation data is not created");
			}
			if (frame < firstFrame || frame > lastFrame) {
				throw new std::exception("frame");
			}
			if (trackIndex < 0 || trackIndex >= trackCount) {
				throw new std::exception("trackIndex");
			}

			animData[frame - firstFrame][trackIndex] = transform;
		}


		DirectX::SimpleMath::Matrix GetAnimKey(int frame, int trackIndex)
		{
			if (animData == nullptr) {
				throw new std::exception("Animation data is not created");
			}
			if (frame < firstFrame || frame > lastFrame) {
				throw new std::exception("frame");
			}
			if (trackIndex < 0 || trackIndex >= trackCount) {
				throw new std::exception("trackIndex");
			}

			return animData[frame - firstFrame][trackIndex];
		}

	};

}
