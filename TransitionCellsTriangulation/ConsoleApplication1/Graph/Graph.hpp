#pragma once
#include <memory>
#include <unordered_set>
#include "../Points/EdgePoint.hpp"
#include "../types.hpp"
#include "../Math/AxisAlighnedBoundingBox.hpp"
#include "../Points/EdgePointsGraph.hpp"

class Graph
{
public:
	Graph();
	Graph(const Graph& other) = delete;
	Graph& operator=(Graph& other) = delete;

	std::shared_ptr<VertexPoint> GetEntry();

	void AddNode(std::shared_ptr<VertexPoint> node);
	
	std::shared_ptr<std::vector<std::shared_ptr<VertexPoint>>> GetNodes();
	std::shared_ptr<std::vector<std::shared_ptr<DoupletVertexPoint>>> GetDoupletNodes();
	
	std::shared_ptr<EdgePoint> GetEdgePoint(int firstVertex, int secondVertex);

	bool static isOnSameFace(int firstEdgePoint, int secondEdgePoint);

	virtual void Create(const VertexActivityMask& vertexActivityMask);

	virtual bool IsTriangleInsideProhibitedArea(Vector3 point1, Vector3 point2, Vector3 point3) const;

	virtual bool IsProhibited(int edge1Index, int edge2Index) const;

	virtual void RemoveRedundantLinks(EdgePointsGraph* edgePointsGraph);

protected:

	struct pairHasher
	{
		size_t operator ()(const std::pair<int, int>& pair) const
		{
			return std::hash<int>()(pair.first) ^ (std::hash<int>()(pair.second) << 1);
		}
	};

	std::unique_ptr<std::unordered_map<std::pair<int, int>, std::shared_ptr<EdgePoint>, pairHasher>> edgesTable = nullptr;
	static std::unique_ptr<std::unordered_map<int, std::unordered_set<int>>> edgeToFaceTable;

	std::shared_ptr<std::vector<std::shared_ptr<VertexPoint>>> nodes;
	std::shared_ptr<std::vector<std::shared_ptr<DoupletVertexPoint>>> doupletNodes;

	AABB prohibitedAreaBoundingBox;
};