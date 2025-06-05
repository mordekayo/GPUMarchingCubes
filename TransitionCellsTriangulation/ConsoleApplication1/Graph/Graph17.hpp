#include "Graph.hpp"
#include "../types.hpp"

class Graph17 : public Graph
{
public:
	Graph17();

	void Create(const VertexActivityMask& vertexActivityMask) override;
	virtual bool IsProhibited(int edge1Index, int edge2Index) const override;

	virtual void RemoveRedundantLinks(EdgePointsGraph* edgePointsGraph) override;
};