#include "table.hpp"
#include "Windows.h"

#include <fstream>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <deque>
#include <set>
#include <stack>
#include "../Math/Vector3.hpp"
#include "../Points/EdgePoint.hpp"
#include "../Graph/Graph.hpp"
#include "../Points/EdgePointsGraph.hpp"

Table::Table()
{

}

void Table::CreateEmpty()
{
    table = std::make_unique<TableArray>();
}

void Table::Serialize(const std::string& fileName)
{
    if (table == nullptr)
    {
        MessageBox(NULL, L"Table is null!", L"ERROR!", MB_ICONERROR | MB_OK);
        return;
    }
    std::string filename{ fileName };
    std::fstream fileStream{ filename, fileStream.binary | fileStream.trunc | fileStream.out };

    if (!fileStream.is_open())
    {
        std::cout << "Failed to open " << filename << '\n';
        return;
    }

    for (int i = 0; i < table->size(); ++i)
    {
        TableRow& row = (*table)[i];
        fileStream.write(reinterpret_cast<char*>(row.data()), sizeof(row.data()));
    }

    fileStream.close();
}

void Table::SerializeAsText(const std::string& fileName)
{
    if (table == nullptr)
    {
        MessageBox(NULL, L"Table is null!", L"ERROR!", MB_ICONERROR | MB_OK);
        return;
    }
    std::string filename{ fileName };
    std::fstream fileStream{ filename, fileStream.binary | fileStream.trunc | fileStream.out };

    if (!fileStream.is_open())
    {
        std::cout << "Failed to open " << filename << '\n';
        return;
    }

    for (int i = 0; i < table->size(); ++i)
    {
        TableRow& row = (*table)[i];
        std::string str;
        for (std::int8_t item : row)
        {
            str += std::to_string(item) + " ,";
        }
        str += "\n";
        fileStream << str;
    }

    fileStream.close();
}

void Table::Deserialize(const std::string& fileName)
{
    std::string filename{ fileName };
    std::fstream fileStream{ filename, fileStream.binary | fileStream.in };

    if (!fileStream.is_open())
    {
        std::cout << "Failed to open " << filename << '\n';
        return;
    }

    failedCount = 0;
    for (int i = 0; i < table->size(); ++i)
    {
        TableRow row;
        fileStream.read(reinterpret_cast<char*>(row.data()), sizeof(row.data()));
    }
}

float Table::GetFailedRatio()
{
    return static_cast<float>(failedCount / tableSize);
}

void Table::PrintFailedRatio()
{
    std::cout << failedCount << "/" << tableSize << " " << GetFailedRatio() << "%\n";
}

void Table::Fill()
{
    for (unsigned long i = 0; i < tableSize; ++i)
    {
        auto vertexActivityMaskCopy = VertexActivityMask(i);
        if (vertexActivityMaskCopy.count() > vertexActivityMaskCopy.size() / 2)
        {
            vertexActivityMaskCopy.flip();
        }
        (*table)[i] = MakeRow(vertexActivityMaskCopy);
    }
}

std::vector<std::uint8_t> EarClipping(const Graph& graph, std::vector<std::shared_ptr<EdgePoint>>& chain)
{
    std::vector<std::uint8_t> triangeList;
    std::vector<std::shared_ptr<EdgePoint>> chainCopy;

    for (auto edgePoint : chain)
    {
        chainCopy.push_back(edgePoint);
    }

    while (chainCopy.size() > 2)
    {
        for (int index = 0; index < chainCopy.size() && chainCopy.size() > 2; ++index)
        {
            int prevIndex = index == 0 ? chainCopy.size() - 1 : index - 1;
            int nextIndex = index == chainCopy.size() - 1 ? 0 : index + 1;

            Vector3 leftArm = chainCopy[prevIndex]->GetPosition() - chainCopy[index]->GetPosition();
            Vector3 rightArm = chainCopy[nextIndex]->GetPosition() - chainCopy[index]->GetPosition();

            if (Vector3::AngleBetween(leftArm, rightArm) < 180.0f)
            {
                if (!graph.IsTriangleInsideProhibitedArea(chainCopy[prevIndex]->GetPosition(),
                    chainCopy[index]->GetPosition(),
                    chainCopy[nextIndex]->GetPosition()))
                {
                    triangeList.push_back(chainCopy[prevIndex]->GetIndex());
                    triangeList.push_back(chainCopy[index]->GetIndex());
                    triangeList.push_back(chainCopy[nextIndex]->GetIndex());
                }
                chainCopy.erase(chainCopy.begin() + index);
                if (prevIndex >= chainCopy.size())
                {
                    prevIndex = chainCopy.size() - 2;
                }
                index = prevIndex;
            }
        }
    }

    return triangeList;
}

void BFS(std::shared_ptr<VertexPoint> vertexPoint, std::unordered_set<std::shared_ptr<VertexPoint>>& alreadyVisitedPoints,
    std::vector<std::shared_ptr<VertexPoint>>& familyPoints)
{
    if (alreadyVisitedPoints.contains(vertexPoint) || !vertexPoint->IsActive())
    {
        return;
    }
    alreadyVisitedPoints.insert(vertexPoint);
    familyPoints.push_back(vertexPoint);
    int neighboursCount = vertexPoint->GetNeighboursCount();
    for (int neighbourIndex = 0; neighbourIndex < neighboursCount; ++neighbourIndex)
    {
        BFS(vertexPoint->GetNeighbour(neighbourIndex), alreadyVisitedPoints, familyPoints);
    }
}

static std::deque<std::shared_ptr<EdgePoint>> ConvertEdgePointsToTChain(const Graph& graph,
    std::vector<std::shared_ptr<EdgePoint>> edgePoints, bool& sucess)
{
    std::deque<std::shared_ptr<EdgePoint>> chain;

    if (!edgePoints.empty())
    {
        chain.push_back(*edgePoints.begin());
        edgePoints.erase(edgePoints.begin());

        while (!edgePoints.empty())
        {
            auto closestPoint = edgePoints.end();
            float distanceToClosestPoint = FLT_MAX;
            bool addToFront = true;
            for (auto it = edgePoints.begin(); it != edgePoints.end(); ++it)
            {
                if (graph.IsProhibited(chain.front()->GetIndex(), (*it)->GetIndex()))
                {
                    continue;
                }
                if (Graph::isOnSameFace(chain.front()->GetIndex(), (*it)->GetIndex()))
                {
                    float distance = Vector3::Distance(chain.front()->GetPosition(), (*it)->GetPosition());
                    if (distance < distanceToClosestPoint)
                    {
                        distanceToClosestPoint = distance;
                        closestPoint = it;
                        addToFront = true;
                    }
                }
                if (Graph::isOnSameFace(chain.back()->GetIndex(), (*it)->GetIndex()))
                {
                    float distance = Vector3::Distance(chain.back()->GetPosition(), (*it)->GetPosition());
                    if (distance < distanceToClosestPoint)
                    {
                        distanceToClosestPoint = distance;
                        closestPoint = it;
                        addToFront = false;
                    }
                }
            }
            if (closestPoint == edgePoints.end())
            {
                sucess = false;
                return chain;
            }
            else
            {
                if (addToFront)
                {
                    chain.push_front(*closestPoint);
                }
                else
                {
                    chain.push_back(*closestPoint);
                }
                edgePoints.erase(closestPoint);
            }
        }
    }

    sucess = true;
    return chain;
}

TableRow Table::MakeRow(const std::vector<int> activeVertexes)
{
    VertexActivityMask vertexActivityMask;
    for (int vertexIndex : activeVertexes)
    {
        vertexActivityMask.set(vertexIndex, true);
    }
    return MakeRow(vertexActivityMask);
}

//void MergeIntersectingEdgePointsFamilies(
//    std::vector<std::unordered_set<std::shared_ptr<EdgePoint>>>& families)
//{
//    bool mergeHappened = false;
//    do
//    {
//        mergeHappened = false;
//        for (int i = families.size() - 1; i > 0; --i)
//        {
//            for (int j = i - 1; j >= 0; --j)
//            {
//                for (auto node : families[i])
//                {
//                    if (families[j].contains(node))
//                    {
//                        families[j].insert(families[i].begin(), families[i].end());
//                        families.erase(families.end() - 1);
//                        mergeHappened = true;
//                        break;
//                    }
//                }
//            }
//        }
//    } while (mergeHappened);
//}

bool Contains(const std::vector<std::shared_ptr<EdgePoint>>& family, std::shared_ptr<EdgePoint> item)
{
    for (auto it : family)
    {
        if (it == item)
        {
            return true;
        }
    }
    return false;
}

EdgePointsGraph* CreateEdgePointsGraph(const std::vector<std::shared_ptr<EdgePoint>> edgePointsFamily, const Graph& graph)
{
    EdgePointsGraph* edgePointsGraph = new EdgePointsGraph();
    for (std::shared_ptr<EdgePoint> edgePoint : edgePointsFamily)
    {
        edgePointsGraph->nodes.push_back(edgePoint);
    }
    for (int i = 0; i < edgePointsGraph->nodes.size(); ++i)
    {
        for (int j = i + 1; j < edgePointsGraph->nodes.size(); ++j)
        {
            if (edgePointsGraph->nodes[i] == edgePointsGraph->nodes[j])
            {
                continue;
            }
            if (!graph.IsProhibited(edgePointsGraph->nodes[i]->GetIndex(), edgePointsGraph->nodes[j]->GetIndex()))
            {   
                if (edgePointsGraph->nodes[i]->GetIndex() < edgePointsGraph->nodes[j]->GetIndex())
                {
                    edgePointsGraph->links.push_back({ edgePointsGraph->nodes[i], edgePointsGraph->nodes[j] });
                }
                else
                {
                    edgePointsGraph->links.push_back({ edgePointsGraph->nodes[j], edgePointsGraph->nodes[i] });
                }
            }
        }
    }

    return edgePointsGraph;
}

bool visited(std::shared_ptr<EdgePoint> node, const std::vector<std::shared_ptr<EdgePoint>>& path) 
{
    return std::find(path.begin(), path.end(), node) != path.end();
}

std::vector<std::shared_ptr<EdgePoint>>::iterator findSmallestIndexInPath(std::vector<std::shared_ptr<EdgePoint>>& path)
{
    int smallestIndex = FLT_MAX;
    std::vector<std::shared_ptr<EdgePoint>>::iterator result = path.begin();
    for (auto it = path.begin(); it != path.end(); ++it)
    {
        if ((*it)->GetIndex() < smallestIndex)
        {
            result = it;
        }
    }
    return result;
}

// Function to rotate a cycle such that 
// it starts with the smallest node
std::vector<std::shared_ptr<EdgePoint>> rotateToSmallest(std::vector<std::shared_ptr<EdgePoint>> path)
{
    auto minIt = findSmallestIndexInPath(path);
    std::rotate(path.begin(), minIt, path.end());
    return path;
}

// Function to check if a cycle is new
bool isNew(const std::vector<std::shared_ptr<EdgePoint>>& path, std::vector<std::vector<std::shared_ptr<EdgePoint>>>& cycles)
{
    std::unordered_set<std::shared_ptr<EdgePoint>> pathSet;
    pathSet.insert(path.begin(), path.end());

    for (const auto& cycle : cycles)
    {
        std::unordered_set<std::shared_ptr<EdgePoint>> set;
        set.insert(cycle.begin(), cycle.end());

        if (pathSet == set)
        {
            return false;
        }
    }
    return true;
}

// Function to find new cycles in the graph
void findNewCycles(std::vector<std::shared_ptr<EdgePoint>> path,
    std::vector<std::vector<std::shared_ptr<EdgePoint>>>& graph,
    std::vector<std::vector<std::shared_ptr<EdgePoint>>>& cycles) 
{
    std::shared_ptr<EdgePoint> startNode = path[0];
    std::shared_ptr<EdgePoint> nextNode = nullptr;
    std::vector<std::shared_ptr<EdgePoint>> sub;

    // Visit each edge and node of each edge
    for (std::vector<std::shared_ptr<EdgePoint>>& edge : graph)
    {
        std::shared_ptr<EdgePoint> node1 = edge[0];
        std::shared_ptr<EdgePoint> node2 = edge[1];

        if (startNode == node1 || startNode == node2) 
        {
            nextNode = (node1 == startNode) ? node2 : node1;

            if (!visited(nextNode, path)) 
            {
                // Neighbor node not on path yet
                sub = { nextNode };
                sub.insert(sub.end(), path.begin(), path.end());
                // Explore extended path
                findNewCycles(sub, graph, cycles);
            }
            else if (path.size() > 2 && nextNode == path.back()) 
            {
                if (isNew(path, cycles))
                {
                    cycles.push_back(path);
                }
            }
        }
    }
}

// Main function to initiate cycle detection
std::vector<std::vector<std::shared_ptr<EdgePoint>>> findCycles(std::vector<std::vector<std::shared_ptr<EdgePoint>>>& graph) {

    std::vector<std::vector<std::shared_ptr<EdgePoint>>> cycles;

    for (std::vector<std::shared_ptr<EdgePoint>>& edge : graph)
    {
        for (std::shared_ptr<EdgePoint> node : edge)
        {
            findNewCycles({ node }, graph, cycles);
        }
    }

    return cycles;
}

void sortCircuitsBySize(std::vector<std::vector<std::shared_ptr<EdgePoint>>>& circuits)
{
    struct
    {
        bool operator()(const std::vector<std::shared_ptr<EdgePoint>>& a,
            const std::vector<std::shared_ptr<EdgePoint>>& b) const
        {
            return a.size() < b.size(); 
        }
    }
    customLess;
    std::sort(circuits.begin(), circuits.end(), customLess);
}

bool FloatEqual(float a, float b)
{
    return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
}

void removeFaceAlignedCircuits(std::vector<std::vector<std::shared_ptr<EdgePoint>>>& circuits)
{
    for (int i = circuits.size() - 1; i >= 1; --i)
    {
        std::vector<float> xs;
        std::vector<float> ys;
        std::vector<float> zs;
        
        for (int j = 0; j < circuits[i].size(); ++j)
        {
            Vector3 pos = circuits[i][j]->GetPosition();
            xs.push_back(pos.x);
            ys.push_back(pos.y);
            zs.push_back(pos.z);
        }

        bool allXisZero = true;
        bool allXisOne = true;
        for (auto x : xs)
        {
            if (!FloatEqual(x, 0))
            {
                allXisZero = false;
            }
            if (!FloatEqual(x, 1))
            {
                allXisOne = false;
            }
        }

        bool allYisZero = true;
        bool allYisOne = true;
        for (auto y : ys)
        {
            if (!FloatEqual(y, 0))
            {
                allYisZero = false;
            }
            if (!FloatEqual(y, 1))
            {
                allYisOne = false;
            }
        }

        bool allZisZero = true;
        bool allZisOne = true;
        for (auto z : zs)
        {
            if (!FloatEqual(z, 0))
            {
                allZisZero = false;
            }
            if (!FloatEqual(z, 1))
            {
                allZisOne = false;
            }
        }

        if (allXisZero || allXisOne || allYisZero || allYisOne || allZisZero || allZisOne)
        {
            circuits.erase(circuits.begin() + i);
        }
    }
}

void removeCombinedCircuits(std::vector<std::vector<std::shared_ptr<EdgePoint>>>& circuits)
{
    sortCircuitsBySize(circuits);

    for (int i = circuits.size() - 1; i >= 1; --i)
    {
        bool shouldBeRemoved = true;
        std::unordered_set<std::shared_ptr<EdgePoint>> iset;
        for (auto point : circuits[i])
        {
            iset.insert(point);
        }
    
        std::unordered_set<std::shared_ptr<EdgePoint>> othersSet;
        for (int j = 0; j < i; ++j)
        {
            for (auto point : circuits[j])
            {
                othersSet.insert(point);
            }
        }

        for (auto point : iset)
        {
            if (!othersSet.contains(point))
            {
                shouldBeRemoved = false;
                break;
            }
        }

        if (shouldBeRemoved)
        {
            circuits.erase(circuits.begin() + i);
        }
    }
}

EdgePointsGraph* CombineGraphs(EdgePointsGraph* combinedGraph, EdgePointsGraph* graphToAdd)
{
    if (combinedGraph->nodes.size() == 0)
    {
        delete combinedGraph;
        combinedGraph = nullptr;
        return graphToAdd;
    }

    std::vector<int> addedNodesIndexes;
    int initialCombinedGraphSize = combinedGraph->nodes.size();
    for (auto link : graphToAdd->links)
    {
        combinedGraph->links.push_back(link);
    }
    for (int i = 0; i < graphToAdd->nodes.size(); ++i)
    {
        std::shared_ptr<EdgePoint> taNode = graphToAdd->nodes[i];
        combinedGraph->nodes.push_back(taNode);
        addedNodesIndexes.push_back(i);
        for (int j = 0; j < initialCombinedGraphSize; ++j)
        {
            std::shared_ptr<EdgePoint> cgNode = combinedGraph->nodes[j];
            if (Graph::isOnSameFace(cgNode->GetIndex(), taNode->GetIndex()))
            {
                if (cgNode->GetIndex() < taNode->GetIndex())
                {
                    combinedGraph->links.push_back({ cgNode, taNode });
                }
                else
                {
                    combinedGraph->links.push_back({ taNode, cgNode });
                }
            }
        }
    }

    delete graphToAdd;
    graphToAdd = nullptr;

    return combinedGraph;
}

TableRow Table::MakeRow(const VertexActivityMask& vertexActivityMask)
{
    TableRow tableRow;
    tableRow.fill(-1);
    int index = 0;

    std::unique_ptr<Graph> graph = CreateGraph();
    graph->Create(vertexActivityMask);

    std::shared_ptr<VertexPoint> entry = graph->GetEntry();

    std::unordered_set<std::shared_ptr<VertexPoint>> alreadyVisitedNodes;
    std::vector<EdgePointsGraph*> edgePointsGraphs;
    std::vector<std::vector<std::shared_ptr<EdgePoint>>> closedCircuits;
    for (std::shared_ptr<VertexPoint> node : *(graph->GetNodes()))
    {
        if (node == nullptr)
        {
            continue;
        }
        if (node->IsActive() && !alreadyVisitedNodes.contains(node))
        {
            std::vector<std::shared_ptr<VertexPoint>> vertexPointsFamily;
            BFS(node, alreadyVisitedNodes, vertexPointsFamily);
            for (std::shared_ptr<VertexPoint> vertexPoint : vertexPointsFamily)
            {
                std::vector<std::shared_ptr<EdgePoint>> edgePointsFamily;
                int neighboursCount = vertexPoint->GetNeighboursCount();
                for (int neighbourIndex = 0; neighbourIndex < neighboursCount; ++neighbourIndex)
                {
                    std::shared_ptr<VertexPoint> neighbour = vertexPoint->GetNeighbour(neighbourIndex);
                    if (!neighbour->IsActive())
                    {
                        auto edgePoint = graph->GetEdgePoint(vertexPoint->GetIndex(), neighbour->GetIndex());
                        int edgePointIndex = edgePoint->GetIndex();
                        if (!Contains(edgePointsFamily, edgePoint))
                        {
                            edgePointsFamily.push_back(edgePoint);
                        }
                    }
                }

                int doupletNeighboursCount = vertexPoint->GetDoupletNeighboursCount();
                for (int doupletNeighbourIndex = 0; doupletNeighbourIndex < doupletNeighboursCount; ++doupletNeighbourIndex)
                {
                    std::shared_ptr<DoupletVertexPoint> doupletNeighbour = vertexPoint->GetDoupletNeighbour(doupletNeighbourIndex);
                    if (!doupletNeighbour->IsActive())
                    {
                        auto edgePoint = graph->GetEdgePoint(vertexPoint->GetIndex(), doupletNeighbour->GetIndex());
                        int edgePointIndex = edgePoint->GetIndex();
                        if (!Contains(edgePointsFamily, edgePoint))
                        {
                            edgePointsFamily.push_back(edgePoint);
                        }
                    }
                }

                if (edgePointsFamily.size() > 0)
                {
                    edgePointsGraphs.push_back(CreateEdgePointsGraph(edgePointsFamily, *graph));
                }
            }
            
            if (edgePointsGraphs.size() > 0)
            {
                EdgePointsGraph* combinedGraph = new EdgePointsGraph();
                for (auto graph : edgePointsGraphs)
                {
                    combinedGraph = CombineGraphs(combinedGraph, graph);
                }
                graph->RemoveRedundantLinks(combinedGraph);
                auto closedCircuits = findCycles(combinedGraph->links);
                removeFaceAlignedCircuits(closedCircuits);
                removeCombinedCircuits(closedCircuits);
                for (auto circuit : closedCircuits)
                {
                    std::vector<std::uint8_t> triangles = EarClipping(*graph, circuit);

                    for (int i = 0; i < triangles.size(); ++i)
                    {
                        tableRow[index++] = triangles[i];
                    }

                    //TODO reverse triangles by position of vertexes that created circuit

                    //if (Vector3::DotProduct(circuitPlaneNormal, chain[0]->GetPosition() - (*edgePointsFamily.begin())->GetPosition()) < 0)
                    //{
                    //    for (auto it = triangles.begin(); it != triangles.end(); ++it)
                    //    {
                    //        tableRow[index++] = *it;
                    //    }
                    //}
                    //else
                    //{
                    //    for (auto rIt = triangles.rbegin(); rIt != triangles.rend(); ++rIt)
                    //    {
                    //        tableRow[index++] = *rIt;
                    //    }
                    //}
                }
            }
        }
    }
    
    if (index > maxEdgePointsCount)
    {
        maxEdgePointsCount = index;
    }

    return tableRow;
}

int Table::GetMaxEdgePointsCount()
{
    return maxEdgePointsCount;
}
