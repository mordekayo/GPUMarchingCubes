#pragma once
#include "VertexPoints/VertexPoint.hpp"

struct Vector3;

class EdgePoint
{
public:
	EdgePoint(std::shared_ptr<IGraphNode> _firstParent, std::shared_ptr<IGraphNode> _secondParent, std::int8_t _index);
	int GetFirstParentIndex();
	int GetSecondParentIndex();
	std::shared_ptr<IGraphNode> GetFirstParent();
	std::shared_ptr<IGraphNode> GetSecondParent();
	//std::shared_ptr<IGraphNode> GetSecondParent();
	std::int8_t GetIndex() const;
	Vector3 GetPosition() const;
	bool IsHighPriority() const;
	void SetHighPriority();
private:
	Vector3 position;
	std::int8_t index;
	bool highPriority = false;
	std::weak_ptr<IGraphNode> firstParent;
	std::weak_ptr<IGraphNode> secondParent;
};