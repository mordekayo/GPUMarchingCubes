#pragma once

#include "GraphNode.hpp"
#include <memory>

class VertexPoint;

class DoupletVertexPoint : public IGraphNode
{
public:
	DoupletVertexPoint(int index, std::shared_ptr<VertexPoint> first, std::shared_ptr<VertexPoint> second);

	virtual bool IsActive() const override;
	virtual int GetIndex() const override;

private:

	int index;
	std::shared_ptr<VertexPoint> node1 = nullptr;
	std::shared_ptr<VertexPoint> node2 = nullptr;
};