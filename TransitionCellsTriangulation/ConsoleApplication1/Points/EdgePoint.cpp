#include "VertexPoints/VertexPoint.hpp"
#include "EdgePoint.hpp"

EdgePoint::EdgePoint(const IGraphNode& firstParent, const IGraphNode& secondParent, std::int8_t _index)
    : index(_index), firstParentIndex(firstParent.GetIndex()), secondParentIndex(secondParent.GetIndex())
{
    float x = (firstParent.GetPosition().x + secondParent.GetPosition().x) / 2;
    float y = (firstParent.GetPosition().y + secondParent.GetPosition().y) / 2;
    float z = (firstParent.GetPosition().z + secondParent.GetPosition().z) / 2;
    position = Vector3(x,y,z);
}

int EdgePoint::GetFirstParentIndex() const
{
    return firstParentIndex;
}

int EdgePoint::GetSecondParentIndex() const
{
    return secondParentIndex;
}

std::int8_t EdgePoint::GetIndex() const
{
    return index;
}

Vector3 EdgePoint::GetPosition() const
{
    return position;
}

bool EdgePoint::IsHighPriority() const
{
    return highPriority;
}

void EdgePoint::SetHighPriority()
{
    highPriority = true;
}
