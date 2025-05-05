#include <iostream>
#include <memory>
#include "Graph.hpp"
#include "../types.hpp"
#include <unordered_map>
#include "../Points/EdgePoint.hpp"
#include "../Points/VertexPoints/VertexPoint.hpp"
#include <format>

std::unique_ptr<std::unordered_map<int, std::unordered_set<int>>> Graph::edgeToFaceTable = nullptr;
std::unique_ptr<std::unordered_map<std::pair<int, int>, std::shared_ptr<EdgePoint>, Graph::pairHasher>> Graph::edgesTable = nullptr;

Graph::Graph()
{

}

std::shared_ptr<VertexPoint> Graph::GetEntry()
{
    return nodes->at(0);
}

void Graph::AddNode(std::shared_ptr<VertexPoint> node)
{
    nodes->at(node->GetIndex()) = node;
}

std::shared_ptr<std::vector<std::shared_ptr<VertexPoint>>> Graph::GetNodes()
{
    return nodes;
}

std::shared_ptr<EdgePoint> Graph::GetEdgePoint(int firstVertexIndex, int secondVertexIndex)
{
    int first = std::min(firstVertexIndex, secondVertexIndex);
	int second = std::max(firstVertexIndex, secondVertexIndex);
    if (edgesTable->count(std::make_pair(first, second)) == 0)
    {
        return {};
    }
    return edgesTable->at(std::make_pair(first, second));
}

bool Graph::isOnSameFace(int firstEdgePoint, int secondEdgePoint)
{
    const auto& facesOfFirstEdgePoint = edgeToFaceTable->at(firstEdgePoint);
    const auto& facesOfSecondEdgePoint = edgeToFaceTable->at(secondEdgePoint);
    for (const auto& face : facesOfFirstEdgePoint)
    {
        if (facesOfSecondEdgePoint.contains(face))
        {
            return true;
        }
    }
    return false;
}

void Graph::Create(const VertexActivityMask& vertexActivityMask)
{

}

bool Graph::IsTriangleInsideProhibitedArea(Vector3 point1, Vector3 point2, Vector3 point3) const
{
    return prohibitedAreaBoundingBox.IsPointInside(
        Vector3((point1.x + point2.x + point3.x) / 3,
        (point1.y + point2.y + point3.y) / 3,
        (point1.z + point2.z + point3.z) / 3));
}

bool Graph::IsProhibited(int edge1Index, int edge2Index) const
{
    return false;
}

void Graph::RemoveRedundantLinks(EdgePointsGraph* edgePointsGraph)
{

}

