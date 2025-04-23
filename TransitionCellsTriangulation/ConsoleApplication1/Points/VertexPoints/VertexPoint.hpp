#pragma once
#include <unordered_map>
#include <memory>

#include "../../Math/Vector3.hpp"
#include "GraphNode.hpp"
#include "DoupletVertexPoint.hpp"

class VertexPoint : public IGraphNode
{
public:
	VertexPoint(Vector3 position, int _index, bool _active);

	int GetNeighboursCount() const;
	std::shared_ptr<VertexPoint> GetNeighbour(int index) const;
	void AddNeighbour(std::shared_ptr<VertexPoint> newNode);

	int GetDoupletNeighboursCount() const;
	std::shared_ptr<DoupletVertexPoint> GetDoupletNeighbour(int index) const;
	void AddDoupletNeighbour(std::shared_ptr<DoupletVertexPoint> newNode);

	Vector3 GetPosition() const;
	
	virtual bool IsActive() const override;
	virtual int GetIndex() const override;

private:
	Vector3 position;
	const int index;
	const bool active;
	std::vector<std::shared_ptr<VertexPoint>> neighbours;
	std::vector<std::shared_ptr<DoupletVertexPoint>> doupletNeighbours;
};