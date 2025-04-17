#pragma once
#include <unordered_map>
#include <memory>

#include "../../Math/Vector3.hpp"

class VertexPoint
{
public:
	VertexPoint(Vector3 position, int _index, bool _active);
	int GetNeighboursCount();
	std::shared_ptr<VertexPoint> GetNeighbour(int index);
	void AddNeighbour(std::shared_ptr<VertexPoint> newNode);
	bool IsActive();
	Vector3 GetPosition() const;
	int GetIndex() const;
private:
	Vector3 position;
	const int index;
	const bool active;
	std::vector<std::shared_ptr<VertexPoint>> neighbours;
};