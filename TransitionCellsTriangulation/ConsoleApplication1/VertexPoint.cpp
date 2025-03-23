#include "VertexPoint.hpp"
#include <iostream>

VertexPoint::VertexPoint(Vector3 _position, int _index, bool _active)
    : position(_position), index(_index), active(_active)
{

}

int VertexPoint::GetNeighboursCount()
{
    return neighbours.size();
}

std::shared_ptr<VertexPoint> VertexPoint::GetNeighbour(int index)
{
    auto b = neighbours[index];
    if (b == nullptr)
    {
        std::cout << "asd";
    }
    return b;
}

void VertexPoint::AddNeighbour(std::shared_ptr<VertexPoint> newNode)
{
    neighbours.push_back(newNode);
}

bool VertexPoint::IsActive()
{
    return active;
}

Vector3 VertexPoint::GetPosition() const
{
    return position;
}

int VertexPoint::GetIndex() const
{
    return index;
}
