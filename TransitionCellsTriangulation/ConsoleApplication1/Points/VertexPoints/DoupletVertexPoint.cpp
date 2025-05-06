#include "DoupletVertexPoint.hpp"
#include "VertexPoint.hpp"
#include "../EdgePoint.hpp"

DoupletVertexPoint::DoupletVertexPoint(int _index, std::shared_ptr<VertexPoint> _first,
	std::shared_ptr<VertexPoint> _second) : index(_index), node1(_first), node2(_second)
{

}

DoupletVertexPoint::DoupletVertexPoint(int _index, std::shared_ptr<VertexPoint> _first,
	std::shared_ptr<VertexPoint> _second, std::shared_ptr<VertexPoint> _third,
	std::shared_ptr<VertexPoint> _fourth) : index(_index), node1(_first), node2(_second),
	node3(_third), node4(_fourth)
{

}

bool DoupletVertexPoint::IsActive() const
{
	if (node3 == nullptr)
	{
		return node1->IsActive() && node2->IsActive();
	}
	else
	{
		return node1->IsActive() && node2->IsActive() && node3->IsActive() && node4->IsActive();
	}
}

int DoupletVertexPoint::GetIndex() const
{
	return index;
}

Vector3 DoupletVertexPoint::GetPosition() const
{
	if (node3 == nullptr)
	{
		Vector3 v = node2->GetPosition() - node1->GetPosition();
		return node1->GetPosition() + v / 2;
	}
	else
	{
		Vector3 v = node3->GetPosition() - node1->GetPosition();
		return node1->GetPosition() + v / 2;
	}
}

//std::vector<std::shared_ptr<IGraphNode>> DoupletVertexPoint::GetParents()
//{
//	if (node3 == nullptr)
//	{
//		return { node1, node2 };
//	}
//	else
//	{
//		return { node1, node2, node3, node4 };
//	}
//}

void DoupletVertexPoint::SetLinkedEdgePoint(std::shared_ptr<EdgePoint> _edgePoint)
{
	edgePoint = _edgePoint;
}

std::shared_ptr<EdgePoint> DoupletVertexPoint::GetLinkedEdgePoint()
{
	return edgePoint;
}
