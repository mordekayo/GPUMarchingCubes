#include "Graph13.hpp"
#include "../Points/VertexPoints/VertexPoint.hpp"
#include "../Math/Vector3.hpp"

Graph13::Graph13()
{

}

void Graph13::Create(const VertexActivityMask& vertexActivityMask)
{
    std::shared_ptr<VertexPoint> node0 = std::make_shared<VertexPoint>(Vector3(0.0f, 0.0f, 0.0f), 0, vertexActivityMask.test(0));
    std::shared_ptr<VertexPoint> node1 = std::make_shared<VertexPoint>(Vector3(2.0f, 0.0f, 0.0f), 1, vertexActivityMask.test(1));
    std::shared_ptr<VertexPoint> node2 = std::make_shared<VertexPoint>(Vector3(2.0f, 0.0f, 1.0f), 2, vertexActivityMask.test(2));
    std::shared_ptr<VertexPoint> node3 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.0f, 1.0f), 3, vertexActivityMask.test(3));
    std::shared_ptr<VertexPoint> node4 = std::make_shared<VertexPoint>(Vector3(0.f, 0.0f, 1.0f), 4, vertexActivityMask.test(4));
    std::shared_ptr<VertexPoint> node5 = std::make_shared<VertexPoint>(Vector3(2.0f, 1.0f, 1.0f), 5, vertexActivityMask.test(5));
    std::shared_ptr<VertexPoint> node6 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 1.0f), 6, vertexActivityMask.test(6));
    std::shared_ptr<VertexPoint> node7 = std::make_shared<VertexPoint>(Vector3(0.0f, 1.0f, 1.0f), 7, vertexActivityMask.test(7));
    std::shared_ptr<VertexPoint> node8 = std::make_shared<VertexPoint>(Vector3(2.0f, 2.0f, 0.0f), 8, vertexActivityMask.test(8));
    std::shared_ptr<VertexPoint> node9 = std::make_shared<VertexPoint>(Vector3(2.0f, 2.0f, 1.0f), 9, vertexActivityMask.test(9));
    std::shared_ptr<VertexPoint> node10 = std::make_shared<VertexPoint>(Vector3(1.0f, 2.0f, 1.0f), 10, vertexActivityMask.test(10));
    std::shared_ptr<VertexPoint> node11 = std::make_shared<VertexPoint>(Vector3(0.0f, 2.0f, 1.0f), 11, vertexActivityMask.test(11));
    std::shared_ptr<VertexPoint> node12 = std::make_shared<VertexPoint>(Vector3(0.0f, 2.0f, 0.0f), 12, vertexActivityMask.test(12));
    
    std::shared_ptr<DoupletVertexPoint> doupletNode13 = std::make_shared<DoupletVertexPoint>(13, node9, node10);
    std::shared_ptr<DoupletVertexPoint> doupletNode14 = std::make_shared<DoupletVertexPoint>(14, node10, node12);
    std::shared_ptr<DoupletVertexPoint> doupletNode15 = std::make_shared<DoupletVertexPoint>(15, node9, node11);
    std::shared_ptr<DoupletVertexPoint> doupletNode16 = std::make_shared<DoupletVertexPoint>(16, node11, node12);
    std::shared_ptr<DoupletVertexPoint> doupletNode17 = std::make_shared<DoupletVertexPoint>(16, node9, node11, node10, node12);

    nodes = std::make_shared<std::vector<std::shared_ptr<VertexPoint>>>(13);

    node0->AddNeighbour(node1);
    node0->AddNeighbour(node4);
    node0->AddNeighbour(node12);
    
    AddNode(node0);

    node1->AddNeighbour(node0);
    node1->AddNeighbour(node2);
    node1->AddNeighbour(node8);

    node1->AddDoupletNeighbour(doupletNode13);

    AddNode(node1);

    node2->AddNeighbour(node1);
    node2->AddNeighbour(node3);
    node2->AddNeighbour(node5);

    AddNode(node2);

    node3->AddNeighbour(node2);
    node3->AddNeighbour(node4);
    node3->AddNeighbour(node6);

    AddNode(node3);

    node4->AddNeighbour(node3);
    node4->AddNeighbour(node0);
    node4->AddNeighbour(node7);

    AddNode(node4);

    node5->AddNeighbour(node2);
    node5->AddNeighbour(node6);
    node5->AddNeighbour(node9);

    AddNode(node5);

    node6->AddNeighbour(node3);
    node6->AddNeighbour(node5);
    node6->AddNeighbour(node7);
    node6->AddNeighbour(node10);

    AddNode(node6);
    
    node7->AddNeighbour(node4);
    node7->AddNeighbour(node6);
    node7->AddNeighbour(node11);

    AddNode(node7);

    node8->AddNeighbour(node1);
    node8->AddNeighbour(node9);
    node8->AddNeighbour(node12);
    
    AddNode(node8);

    node9->AddNeighbour(node5);
    node9->AddNeighbour(node8);
    node9->AddNeighbour(node10);

    AddNode(node9);

    node10->AddNeighbour(node6);
    node10->AddNeighbour(node9);
    node10->AddNeighbour(node11);

    AddNode(node10);

    node11->AddNeighbour(node7);
    node11->AddNeighbour(node10);
    node11->AddNeighbour(node12);

    AddNode(node11);

    node12->AddNeighbour(node0);
    node12->AddNeighbour(node8);
    node12->AddNeighbour(node11);

    AddNode(node12);

    if (edgesTable == nullptr)
    {
        edgesTable = std::make_unique<std::unordered_map<std::pair<int, int>, std::shared_ptr<EdgePoint>, Graph::pairHasher>>();

        std::shared_ptr<EdgePoint> edge0 = std::make_shared<EdgePoint>(*node0, *node1, 0);
        edgesTable->insert(std::make_pair(std::make_pair(edge0->GetFirstParentIndex(), edge0->GetSecondParentIndex()), edge0));

        std::shared_ptr<EdgePoint> edge1 = std::make_shared<EdgePoint>(*node1, *node2, 1);
        edgesTable->insert(std::make_pair(std::make_pair(edge1->GetFirstParentIndex(), edge1->GetSecondParentIndex()), edge1));

        std::shared_ptr<EdgePoint> edge2 = std::make_shared<EdgePoint>(*node0, *node3, 2);
        edgesTable->insert(std::make_pair(std::make_pair(edge2->GetFirstParentIndex(), edge2->GetSecondParentIndex()), edge2));

        std::shared_ptr<EdgePoint> edge3 = std::make_shared<EdgePoint>(*node1, *node4, 3);
        edgesTable->insert(std::make_pair(std::make_pair(edge3->GetFirstParentIndex(), edge3->GetSecondParentIndex()), edge3));

        std::shared_ptr<EdgePoint> edge4 = std::make_shared<EdgePoint>(*node2, *node5, 4);
        edgesTable->insert(std::make_pair(std::make_pair(edge4->GetFirstParentIndex(), edge4->GetSecondParentIndex()), edge4));

        std::shared_ptr<EdgePoint> edge5 = std::make_shared<EdgePoint>(*node3, *node4, 5);
        edgesTable->insert(std::make_pair(std::make_pair(edge5->GetFirstParentIndex(), edge5->GetSecondParentIndex()), edge5));

        std::shared_ptr<EdgePoint> edge6 = std::make_shared<EdgePoint>(*node4, *node5, 6);
        edgesTable->insert(std::make_pair(std::make_pair(edge6->GetFirstParentIndex(), edge6->GetSecondParentIndex()), edge6));

        std::shared_ptr<EdgePoint> edge7 = std::make_shared<EdgePoint>(*node3, *node6, 7);
        edgesTable->insert(std::make_pair(std::make_pair(edge7->GetFirstParentIndex(), edge7->GetSecondParentIndex()), edge7));

        std::shared_ptr<EdgePoint> edge8 = std::make_shared<EdgePoint>(*node4, *node7, 8);
        edgesTable->insert(std::make_pair(std::make_pair(edge8->GetFirstParentIndex(), edge8->GetSecondParentIndex()), edge8));

        std::shared_ptr<EdgePoint> edge9 = std::make_shared<EdgePoint>(*node5, *node8, 9);
        edgesTable->insert(std::make_pair(std::make_pair(edge9->GetFirstParentIndex(), edge9->GetSecondParentIndex()), edge9));

        std::shared_ptr<EdgePoint> edge10 = std::make_shared<EdgePoint>(*node6, *node7, 10);
        edgesTable->insert(std::make_pair(std::make_pair(edge10->GetFirstParentIndex(), edge10->GetSecondParentIndex()), edge10));

        std::shared_ptr<EdgePoint> edge11 = std::make_shared<EdgePoint>(*node7, *node8, 11);
        edgesTable->insert(std::make_pair(std::make_pair(edge11->GetFirstParentIndex(), edge11->GetSecondParentIndex()), edge11));

        std::shared_ptr<EdgePoint> edge12 = std::make_shared<EdgePoint>(*node0, *node9, 12);
        edgesTable->insert(std::make_pair(std::make_pair(edge12->GetFirstParentIndex(), edge12->GetSecondParentIndex()), edge12));

        std::shared_ptr<EdgePoint> edge13 = std::make_shared<EdgePoint>(*node2, *node10, 13);
        edgesTable->insert(std::make_pair(std::make_pair(edge13->GetFirstParentIndex(), edge13->GetSecondParentIndex()), edge13));

        std::shared_ptr<EdgePoint> edge14 = std::make_shared<EdgePoint>(*node6, *node11, 14);
        edgesTable->insert(std::make_pair(std::make_pair(edge14->GetFirstParentIndex(), edge14->GetSecondParentIndex()), edge14));

        std::shared_ptr<EdgePoint> edge15 = std::make_shared<EdgePoint>(*node8, *node12, 15);
        edgesTable->insert(std::make_pair(std::make_pair(edge15->GetFirstParentIndex(), edge15->GetSecondParentIndex()), edge15));

        std::shared_ptr<EdgePoint> edge16 = std::make_shared<EdgePoint>(*node9, *node10, 16);
        edgesTable->insert(std::make_pair(std::make_pair(edge16->GetFirstParentIndex(), edge16->GetSecondParentIndex()), edge16));

        std::shared_ptr<EdgePoint> edge17 = std::make_shared<EdgePoint>(*node9, *node11, 17);
        edgesTable->insert(std::make_pair(std::make_pair(edge17->GetFirstParentIndex(), edge17->GetSecondParentIndex()), edge17));

        std::shared_ptr<EdgePoint> edge18 = std::make_shared<EdgePoint>(*node11, *node12, 18);
        edgesTable->insert(std::make_pair(std::make_pair(edge18->GetFirstParentIndex(), edge18->GetSecondParentIndex()), edge18));

        std::shared_ptr<EdgePoint> edge19 = std::make_shared<EdgePoint>(*node10, *node12, 19);
        edgesTable->insert(std::make_pair(std::make_pair(edge19->GetFirstParentIndex(), edge19->GetSecondParentIndex()), edge19));

        std::shared_ptr<EdgePoint> edge20 = std::make_shared<EdgePoint>(*node1, *doupletNode13, 20);
        edgesTable->insert(std::make_pair(std::make_pair(edge20->GetFirstParentIndex(), edge20->GetSecondParentIndex()), edge20));

        std::shared_ptr<EdgePoint> edge21 = std::make_shared<EdgePoint>(*node5, *doupletNode14, 21);
        edgesTable->insert(std::make_pair(std::make_pair(edge21->GetFirstParentIndex(), edge21->GetSecondParentIndex()), edge21));

        std::shared_ptr<EdgePoint> edge22 = std::make_shared<EdgePoint>(*node3, *doupletNode15, 22);
        edgesTable->insert(std::make_pair(std::make_pair(edge22->GetFirstParentIndex(), edge22->GetSecondParentIndex()), edge22));

        std::shared_ptr<EdgePoint> edge23 = std::make_shared<EdgePoint>(*node7, *doupletNode16, 23);
        edgesTable->insert(std::make_pair(std::make_pair(edge23->GetFirstParentIndex(), edge23->GetSecondParentIndex()), edge23));

        std::shared_ptr<EdgePoint> edge24 = std::make_shared<EdgePoint>(*node4, *doupletNode17, 24);
        edgesTable->insert(std::make_pair(std::make_pair(edge24->GetFirstParentIndex(), edge24->GetSecondParentIndex()), edge24));
    }
    if (edgeToFaceTable == nullptr)
    {
        edgeToFaceTable = std::make_unique<std::unordered_map<int, std::unordered_set<int>>>();

        edgeToFaceTable->insert(std::make_pair(0, std::unordered_set<int>{0, 11}));
        edgeToFaceTable->insert(std::make_pair(1, std::unordered_set<int>{1, 10}));
        edgeToFaceTable->insert(std::make_pair(2, std::unordered_set<int>{0, 4}));
        edgeToFaceTable->insert(std::make_pair(3, std::unordered_set<int>{0, 1, 20, 22}));
        edgeToFaceTable->insert(std::make_pair(4, std::unordered_set<int>{1, 6}));
        edgeToFaceTable->insert(std::make_pair(5, std::unordered_set<int>{0, 2, 18}));
        edgeToFaceTable->insert(std::make_pair(6, std::unordered_set<int>{1, 3, 19}));
        edgeToFaceTable->insert(std::make_pair(7, std::unordered_set<int>{2, 5}));
        edgeToFaceTable->insert(std::make_pair(8, std::unordered_set<int>{2, 3, 17, 21}));
        edgeToFaceTable->insert(std::make_pair(9, std::unordered_set<int>{3, 7}));
        edgeToFaceTable->insert(std::make_pair(10, std::unordered_set<int>{2, 8}));
        edgeToFaceTable->insert(std::make_pair(11, std::unordered_set<int>{3, 9}));
        edgeToFaceTable->insert(std::make_pair(12, std::unordered_set<int>{4, 11}));
        edgeToFaceTable->insert(std::make_pair(13, std::unordered_set<int>{6, 10}));
        edgeToFaceTable->insert(std::make_pair(14, std::unordered_set<int>{5, 8}));
        edgeToFaceTable->insert(std::make_pair(15, std::unordered_set<int>{7, 9}));
        edgeToFaceTable->insert(std::make_pair(16, std::unordered_set<int>{10, 11, 12}));
        edgeToFaceTable->insert(std::make_pair(17, std::unordered_set<int>{4, 5, 12}));      
        edgeToFaceTable->insert(std::make_pair(18, std::unordered_set<int>{8, 9, 12}));
        edgeToFaceTable->insert(std::make_pair(19, std::unordered_set<int>{6, 7, 12}));
        edgeToFaceTable->insert(std::make_pair(20, std::unordered_set<int>{10, 11, 13, 22}));
        edgeToFaceTable->insert(std::make_pair(21, std::unordered_set<int>{6, 7, 15}));
        edgeToFaceTable->insert(std::make_pair(22, std::unordered_set<int>{4, 5, 14}));
        edgeToFaceTable->insert(std::make_pair(23, std::unordered_set<int>{8, 9, 16, 21}));
        edgeToFaceTable->insert(std::make_pair(24, std::unordered_set<int>{13, 14, 15, 16, 17, 18, 19, 20}));
    }
}

bool Graph13::IsTriangleInsideProhibitedArea(Vector3 point1, Vector3 point2, Vector3 point3) const
{
    return false;
}

bool Graph13::IsProhibited(int edge1Index, int edge2Index) const
{
    if (edge1Index > edge2Index)
    {
        std::swap(edge1Index, edge2Index);
    }
    if (edge1Index == 5 && edge2Index == 6)
    {
        return true;
    }
    if (edge1Index == 3 && edge2Index == 8)
    {
        return true;
    }
    if (edge1Index == 5 && edge2Index == 6)
    {
        return true;
    }
    if (edge1Index == 2 && edge2Index == 7)
    {
        return true;
    }
    if (edge1Index == 0 && edge2Index == 1)
    {
        return true;
    }
    if (edge1Index == 4 && edge2Index == 9)
    {
        return true;
    }
    if (edge1Index == 10 && edge2Index == 11)
    {
        return true;
    }
    return false;
}

std::vector<std::vector<std::shared_ptr<EdgePoint>>>::iterator FindLinkInEdgePointsGraph(
                    EdgePointsGraph* graph, std::vector<int> link)
{
    for (auto it = graph->links.begin(); it != graph->links.end(); ++it)
    {
        if (it->at(0)->GetIndex() == link[0] && it->at(1)->GetIndex() == link[1])
        {
            return it;
        }
    }

    return graph->links.end();
}

void Graph13::RemoveRedundantLinks(EdgePointsGraph* edgePointsGraph)
{
    std::vector<int> link0 = {17, 22};
    if(FindLinkInEdgePointsGraph(edgePointsGraph, link0) != edgePointsGraph->links.end())
    {
        std::vector<int> link1 = { 7, 17 };
        std::vector<int> link2 = { 2, 7 };
        auto link1it = FindLinkInEdgePointsGraph(edgePointsGraph, link1);
        if(link1it != edgePointsGraph->links.end())
        {
            edgePointsGraph->links.erase(link1it);
        }
        auto link2it = FindLinkInEdgePointsGraph(edgePointsGraph, link2);
        if (link2it != edgePointsGraph->links.end())
        {
            edgePointsGraph->links.erase(link2it);
        }
    }

    std::vector<int> link3 = { 18, 23 };
    if (FindLinkInEdgePointsGraph(edgePointsGraph, link3) != edgePointsGraph->links.end())
    {
        std::vector<int> link4 = { 10, 18 };
        std::vector<int> link5 = { 10, 11 };
        auto link4it = FindLinkInEdgePointsGraph(edgePointsGraph, link4);
        if (link4it != edgePointsGraph->links.end())
        {
            edgePointsGraph->links.erase(link4it);
        }
        auto link5it = FindLinkInEdgePointsGraph(edgePointsGraph, link5);
        if (link5it != edgePointsGraph->links.end())
        {
            edgePointsGraph->links.erase(link5it);
        }
    }

    std::vector<int> link6 = { 19, 21 };
    if (FindLinkInEdgePointsGraph(edgePointsGraph, link6) != edgePointsGraph->links.end())
    {
        std::vector<int> link7 = { 9, 19 };
        std::vector<int> link8 = { 4, 19 };
        auto link7it = FindLinkInEdgePointsGraph(edgePointsGraph, link7);
        if (link7it != edgePointsGraph->links.end())
        {
            edgePointsGraph->links.erase(link7it);
        }
        auto link8it = FindLinkInEdgePointsGraph(edgePointsGraph, link8);
        if (link8it != edgePointsGraph->links.end())
        {
            edgePointsGraph->links.erase(link8it);
        }
    }

    std::vector<int> link9 = { 16, 20 };
    if (FindLinkInEdgePointsGraph(edgePointsGraph, link9) != edgePointsGraph->links.end())
    {
        std::vector<int> link10 = { 0, 16 };
        std::vector<int> link11 = { 1, 16 };
        auto link10it = FindLinkInEdgePointsGraph(edgePointsGraph, link10);
        if (link10it != edgePointsGraph->links.end())
        {
            edgePointsGraph->links.erase(link10it);
        }
        auto link11it = FindLinkInEdgePointsGraph(edgePointsGraph, link11);
        if (link11it != edgePointsGraph->links.end())
        {
            edgePointsGraph->links.erase(link11it);
        }
    }
}
