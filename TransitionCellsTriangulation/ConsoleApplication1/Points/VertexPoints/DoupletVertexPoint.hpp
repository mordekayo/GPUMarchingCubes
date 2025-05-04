#pragma once

#include "GraphNode.hpp"
#include <memory>

class VertexPoint;
class Vector3;

class DoupletVertexPoint : public IGraphNode
{
public:
	DoupletVertexPoint(int index, std::shared_ptr<VertexPoint> first, std::shared_ptr<VertexPoint> second);
	DoupletVertexPoint(int index, std::shared_ptr<VertexPoint> first, std::shared_ptr<VertexPoint> second,
		std::shared_ptr<VertexPoint> third, std::shared_ptr<VertexPoint> fourth);

	virtual bool IsActive() const override;
	virtual int GetIndex() const override;
	virtual Vector3 GetPosition() const override;

private:

	int index;
	std::shared_ptr<VertexPoint> node1 = nullptr;
	std::shared_ptr<VertexPoint> node2 = nullptr;
	std::shared_ptr<VertexPoint> node3 = nullptr;
	std::shared_ptr<VertexPoint> node4 = nullptr;
};