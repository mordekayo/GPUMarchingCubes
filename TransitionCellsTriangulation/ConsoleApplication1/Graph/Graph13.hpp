#include "Graph.hpp"
#include "../types.hpp"

class Graph13 : public Graph
{
public:
	Graph13();

	void Create(const VertexActivityMask& vertexActivityMask) override;

	bool IsTriangleInsideProhibitedArea(Vector3 point1, Vector3 point2, Vector3 point3) const override;

	virtual bool IsProhibited(int edge1Index, int edge2Index) const override;

	virtual void RemoveRedundantLinks(EdgePointsGraph* edgePointsGraph) override;
};