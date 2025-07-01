#include "VertexPoints/VertexPoint.hpp"
#include "EdgePoint.hpp"

EdgePoint::EdgePoint(std::shared_ptr<IGraphNode> _firstParent, std::shared_ptr<IGraphNode> _secondParent, std::int8_t _index) 
                    : index(_index), firstParent(_firstParent), secondParent(_secondParent)
{
    float x = (_firstParent->GetPosition().x + _secondParent->GetPosition().x) / 2;
    float y = (_firstParent->GetPosition().y + _secondParent->GetPosition().y) / 2;
    float z = (_firstParent->GetPosition().z + _secondParent->GetPosition().z) / 2;
    position = Vector3(x, y, z);
}

int EdgePoint::GetFirstParentIndex()
{
    std::shared_ptr<IGraphNode> node = firstParent.lock();
    if (!node)
    {
        std::abort();
    }
    return node->GetIndex();
}

int EdgePoint::GetSecondParentIndex()
{
    std::shared_ptr<IGraphNode> node = secondParent.lock();
    if (!node)
    {
        std::abort();
    }
    return node->GetIndex();
}

std::shared_ptr<IGraphNode> EdgePoint::GetFirstParent()
{
    std::shared_ptr<IGraphNode> node = firstParent.lock();
    if (!node)
    {
        std::abort();
    }
    return node;
}

std::shared_ptr<IGraphNode> EdgePoint::GetSecondParent()
{
    std::shared_ptr<IGraphNode> node = secondParent.lock();
    if (!node)
    {
        std::abort();
    }
    return node;
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
