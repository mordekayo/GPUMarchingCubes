#pragma once
#include "Graph.hpp"

class Graph20 : public Graph
{
public:
	Graph20();

	void Create(const VertexActivityMask& vertexActivityMask) override;
};