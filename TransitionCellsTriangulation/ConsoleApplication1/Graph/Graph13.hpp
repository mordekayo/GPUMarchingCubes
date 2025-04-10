#include "Graph.hpp"
#include "../types.hpp"

class Graph13 : public Graph
{
public:
	Graph13();

	void Create(const VertexActivityMask& vertexActivityMask) override;
};