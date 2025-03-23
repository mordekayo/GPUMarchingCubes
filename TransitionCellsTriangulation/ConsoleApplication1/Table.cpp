#include "table.hpp"
#include "Windows.h"

#include <fstream>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <deque>
#include <set>
#include <stack>

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

void Table::Deserialize(const std::string& fileName)
{
    std::string filename{ fileName };
    std::fstream fileStream{ filename, fileStream.binary | fileStream.in };

    if (!fileStream.is_open())
    {
        std::cout << "Failed to open " << filename << '\n';
        return;
    }

    filledCount = 0;
    for (int i = 0; i < table->size(); ++i)
    {
        TableRow row;
        fileStream.read(reinterpret_cast<char*>(row.data()), sizeof(row.data()));
    }
}

float Table::GetFilledRatio()
{
    return static_cast<float>(filledCount / tableSize);
}

void Table::PrintFilledRatio()
{
    std::cout << filledCount << "/" << tableSize << " " << GetFilledRatio() << "%\n";
}

void Table::Fill()
{
    for (unsigned long i = 0; i < vertexActivityMaskLength; ++i)
    {
        (*table)[i] = MakeRow(VertexActivityMask(i));
    }
}

void RemoveExpandedAngles(std::deque<std::shared_ptr<EdgePoint>>& chain)
{
    std::stack<int> toRemove;

    for (size_t index = 0; index < chain.size(); ++index)
    {
        size_t prevIndex = index == 0 ? chain.size() - 1 : index - 1;
        size_t nextIndex = index == chain.size() - 1 ? 0 : index + 1;

        Vector3 leftArm = chain[prevIndex]->GetPosition() - chain[index]->GetPosition();
        Vector3 rightArm = chain[nextIndex]->GetPosition() - chain[index]->GetPosition();

        if (Vector3::AngleBetween(leftArm, rightArm) == 180.0f)
        {
            toRemove.push(index);
        }
    }

    while (!toRemove.empty())
    {
        chain.erase(chain.begin() + toRemove.top());
        toRemove.pop();
    }
}

std::vector<std::uint8_t> EarClipping(std::deque<std::shared_ptr<EdgePoint>>& chain)
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
                triangeList.push_back(chainCopy[prevIndex]->GetIndex());
                triangeList.push_back(chainCopy[index]->GetIndex());
                triangeList.push_back(chainCopy[nextIndex]->GetIndex());
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
    std::unordered_set<std::shared_ptr<VertexPoint>>& familyPoints)
{
    if (alreadyVisitedPoints.contains(vertexPoint) || !vertexPoint->IsActive())
    {
        return;
    }
    alreadyVisitedPoints.insert(vertexPoint);
    familyPoints.insert(vertexPoint);
    int neighboursCount = vertexPoint->GetNeighboursCount();
    for (int neighbourIndex = 0; neighbourIndex < neighboursCount; ++neighbourIndex)
    {
        BFS(vertexPoint->GetNeighbour(neighbourIndex), alreadyVisitedPoints, familyPoints);
    }
}

static std::deque<std::shared_ptr<EdgePoint>> ConvertEdgePointsToTChain(std::set<std::shared_ptr<EdgePoint>> edgePoints)
{
    std::deque<std::shared_ptr<EdgePoint>> chain;

    if (!edgePoints.empty())
    {
        chain.push_back(*edgePoints.begin());
        edgePoints.erase(edgePoints.begin());

        while (!edgePoints.empty())
        {
            std::shared_ptr<EdgePoint> closestPoint = nullptr;
            float distanceToClosestPoint = FLT_MAX;
            bool addToFront = true;
            for (auto it = edgePoints.begin(); it != edgePoints.end(); ++it)
            {
                if (Graph::isOnSameFace(chain.front()->GetIndex(), (*it)->GetIndex()))
                {
                    float distance = Vector3::Distance(chain.front()->GetPosition(), (*it)->GetPosition());
                    if (distance < distanceToClosestPoint)
                    {
                        distanceToClosestPoint = distance;
                        closestPoint = *it;
                        addToFront = true;
                    }
                }
                if (Graph::isOnSameFace(chain.back()->GetIndex(), (*it)->GetIndex()))
                {
                    float distance = Vector3::Distance(chain.back()->GetPosition(), (*it)->GetPosition());
                    if (distance < distanceToClosestPoint)
                    {
                        distanceToClosestPoint = distance;
                        closestPoint = *it;
                        addToFront = false;
                    }
                }
            }
            if (addToFront)
            {
                chain.push_front(closestPoint);
            }
            else
            {
                chain.push_back(closestPoint);
            }
            edgePoints.erase(closestPoint);
        }
    }

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

TableRow Table::MakeRow(const VertexActivityMask& vertexActivityMask)
{
    TableRow tableRow;
    tableRow.fill(-1);
    int index = 0;

    std::unique_ptr<Graph> graph = std::make_unique<Graph>(vertexActivityMask);

    std::shared_ptr<VertexPoint> entry = graph->GetEntry();

    std::unordered_set<std::shared_ptr<VertexPoint>> alreadyVisitedNodes;

    for (std::shared_ptr<VertexPoint> node : *(graph->GetNodes()))
    {
        if (node == nullptr)
        {
            continue;
        }
        if (node->IsActive() && !alreadyVisitedNodes.contains(node))
        {
            std::set<std::shared_ptr<EdgePoint>> edgePoints;
            std::unordered_set<std::shared_ptr<VertexPoint>> familyPoints;
            BFS(node, alreadyVisitedNodes, familyPoints);
            for (std::shared_ptr<VertexPoint> child : familyPoints)
            {
                int neighboursCount = child->GetNeighboursCount();
                for (int neighbourIndex = 0; neighbourIndex < neighboursCount; ++neighbourIndex)
                {
                    std::shared_ptr<VertexPoint> neighbour = child->GetNeighbour(neighbourIndex);
                    if (!neighbour->IsActive())
                    {
                        auto edgePoint = graph->GetEdgePoint(child->GetIndex(), neighbour->GetIndex());
                        edgePoints.insert(edgePoint);
                    }
                }
            }

            std::deque<std::shared_ptr<EdgePoint>> chain = ConvertEdgePointsToTChain(edgePoints);

            Vector3 chainPlaneNormal = Vector3::CrossProduct(chain[1]->GetPosition() - chain[0]->GetPosition(),
                chain[2]->GetPosition() - chain[1]->GetPosition());

            auto triangles = EarClipping(chain);



            if (Vector3::DotProduct(chainPlaneNormal, chain[0]->GetPosition() - node->GetPosition()) < 0)
            {
                for (auto it = triangles.begin(); it != triangles.end(); ++it)
                {
                    tableRow[index++] = *it;
                }
            }
            else
            {
                for (auto rIt = triangles.rbegin(); rIt != triangles.rend(); ++rIt)
                {
                    tableRow[index++] = *rIt;
                }
            }
        }
    }

    return tableRow;
}
