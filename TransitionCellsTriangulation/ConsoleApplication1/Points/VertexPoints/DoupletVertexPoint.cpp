#include "DoupletVertexPoint.hpp"
#include "VertexPoint.hpp"

DoupletVertexPoint::DoupletVertexPoint(int _index, std::shared_ptr<VertexPoint> _first,
	std::shared_ptr<VertexPoint> _second) : index(_index), node1(_first), node2(_second)
{

}

bool DoupletVertexPoint::IsActive() const
{
	return node1->IsActive() && node2->IsActive();
}

int DoupletVertexPoint::GetIndex() const
{
	return index;
}
