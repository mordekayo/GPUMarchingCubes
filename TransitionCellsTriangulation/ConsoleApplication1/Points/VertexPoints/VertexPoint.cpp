#include <iostream>
#include "VertexPoint.hpp"

VertexPoint::VertexPoint(Vector3 _position, int _index, bool _active)
    : position(_position), index(_index), active(_active)
{

}

int VertexPoint::GetNeighboursCount() const
{
    return neighbours.size();
}

std::shared_ptr<VertexPoint> VertexPoint::GetNeighbour(int index) const
{
    return neighbours[index];
}

void VertexPoint::AddNeighbour(std::shared_ptr<VertexPoint> newNode)
{
    neighbours.push_back(newNode);
}

int VertexPoint::GetDoupletNeighboursCount() const
{
    return doupletNeighbours.size();
}

std::shared_ptr<DoupletVertexPoint> VertexPoint::GetDoupletNeighbour(int index) const
{
    return doupletNeighbours[index];
}

void VertexPoint::AddDoupletNeighbour(std::shared_ptr<DoupletVertexPoint> newNode)
{
    doupletNeighbours.push_back(newNode);
}

bool VertexPoint::IsActive() const
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
