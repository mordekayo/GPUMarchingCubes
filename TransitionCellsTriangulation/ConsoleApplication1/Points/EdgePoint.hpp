#pragma once
#include "VertexPoints/VertexPoint.hpp"

struct Vector3;

class EdgePoint
{
public:
	EdgePoint(const VertexPoint& firstParent, const VertexPoint& secondParent, std::int8_t _index);
	int GetFirstParentIndex() const;
	int GetSecondParentIndex() const;
	std::int8_t GetIndex() const;
	Vector3 GetPosition() const;
	bool IsHighPriority() const;
	void SetHighPriority();
private:
	Vector3 position;
	std::int8_t index;
	int firstParentIndex;
	int secondParentIndex;
	bool highPriority = false;
};