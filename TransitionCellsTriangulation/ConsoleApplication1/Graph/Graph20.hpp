#pragma once
#include "Graph.hpp"

class Graph20 : public Graph
{
public:
	Graph20();

	void Create(const VertexActivityMask& vertexActivityMask) override;
	virtual bool IsProhibited(int edge1Index, int edge2Index) const override;

	virtual void RemoveRedundantLinks(EdgePointsGraph* edgePointsGraph) override;
};