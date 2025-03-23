#include <memory>
#include <array>
#include <unordered_set>

#include "types.hpp"
#include "EdgePoint.hpp"

class Graph
{
public:
	Graph(const VertexActivityMask& vertexActivityMask);
	Graph(const Graph& other) = delete;
	Graph& operator=(Graph& other) = delete;

	std::shared_ptr<VertexPoint> GetEntry();

	void AddNode(std::shared_ptr<VertexPoint> node);
	
	std::shared_ptr<std::array<std::shared_ptr<VertexPoint>, 20>> GetNodes();
	
	std::shared_ptr<EdgePoint> GetEdgePoint(int firstVertex, int secondVertex);

	bool static isOnSameFace(int firstEdgePoint, int secondEdgePoint);

private:

	struct pairHasher
	{
		size_t operator ()(const std::pair<int, int>& pair) const
		{
			return std::hash<int>()(pair.first) ^ (std::hash<int>()(pair.second) << 1);
		}
	};
	static std::unique_ptr<std::unordered_map<std::pair<int, int>, std::shared_ptr<EdgePoint>, pairHasher>> edgesTable;
	static std::unique_ptr<std::unordered_map<int, std::unordered_set<int>>> edgeToFaceTable;
	std::shared_ptr<std::array<std::shared_ptr<VertexPoint>, 20>> nodes;
};