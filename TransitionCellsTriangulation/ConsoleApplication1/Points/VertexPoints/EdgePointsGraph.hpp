#pragma once

struct EdgePointsGraphNode
{
	EdgePointsGraphNode(int _edgePointIndex) : edgePointIndex(_edgePointIndex) {}
	int edgePointIndex;
	std::unordered_set<EdgePointsGraphNode*> neighbours;
};

struct EdgePointsGraph
{
	EdgePointsGraphNode* root;
	std::vector<EdgePointsGraphNode*> nodes;
};