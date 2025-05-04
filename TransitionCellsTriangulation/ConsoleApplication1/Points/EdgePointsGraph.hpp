#pragma once

class EdgePoint;

struct EdgePointsGraph
{
	std::vector<std::shared_ptr<EdgePoint>> nodes;
	std::vector<std::vector<std::shared_ptr<EdgePoint>>> links;
};