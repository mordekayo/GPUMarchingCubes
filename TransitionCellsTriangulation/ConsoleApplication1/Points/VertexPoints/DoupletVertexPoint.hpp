#pragma once

#include "GraphNode.hpp"
#include <memory>
#include <vector>

class VertexPoint;
class Vector3;
class EdgePoint;

class DoupletVertexPoint : public IGraphNode
{
public:
	DoupletVertexPoint(int index, std::shared_ptr<VertexPoint> first, std::shared_ptr<VertexPoint> second);
	DoupletVertexPoint(int index, std::shared_ptr<VertexPoint> first, std::shared_ptr<VertexPoint> second,
		std::shared_ptr<VertexPoint> third, std::shared_ptr<VertexPoint> fourth);

	virtual bool IsActive() const override;
	virtual int GetIndex() const override;
	virtual Vector3 GetPosition() const override;

	void SetLinkedEdgePoint(std::shared_ptr<EdgePoint> edgePoint);
	std::shared_ptr<EdgePoint> GetLinkedEdgePoint();
		
	std::vector<std::shared_ptr<VertexPoint>> GetParents();

private:

	int index;
	std::shared_ptr<VertexPoint> node1 = nullptr;
	std::shared_ptr<VertexPoint> node2 = nullptr;
	std::shared_ptr<VertexPoint> node3 = nullptr;
	std::shared_ptr<VertexPoint> node4 = nullptr;

	std::shared_ptr<EdgePoint> edgePoint = nullptr;
};