#include "Graph17.hpp"
#include "../Points/VertexPoints/VertexPoint.hpp"
#include "../Math/Vector3.hpp"

Graph17::Graph17()
{

}

void Graph17::Create(const VertexActivityMask& vertexActivityMask)
{
    std::shared_ptr<VertexPoint> node0 = std::make_shared<VertexPoint>(Vector3(0.0f, 0.0f, 0.0f), 0, vertexActivityMask.test(0));
    std::shared_ptr<VertexPoint> node1 = std::make_shared<VertexPoint>(Vector3(2.0f, 0.0f, 0.0f), 1, vertexActivityMask.test(1));
    std::shared_ptr<VertexPoint> node2 = std::make_shared<VertexPoint>(Vector3(2.0f, 0.0f, 1.0f), 2, vertexActivityMask.test(2));
    std::shared_ptr<VertexPoint> node3 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.0f, 1.0f), 3, vertexActivityMask.test(3));
    std::shared_ptr<VertexPoint> node4 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.0f, 2.0f), 4, vertexActivityMask.test(4));
    std::shared_ptr<VertexPoint> node5 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.0f, 2.0f), 5, vertexActivityMask.test(5));
    std::shared_ptr<VertexPoint> node6 = std::make_shared<VertexPoint>(Vector3(2.0f, 1.0f, 1.0f), 6, vertexActivityMask.test(6));
    std::shared_ptr<VertexPoint> node7 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 1.0f), 7, vertexActivityMask.test(7));
    std::shared_ptr<VertexPoint> node8 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 2.0f), 8, vertexActivityMask.test(8));
    std::shared_ptr<VertexPoint> node9 = std::make_shared<VertexPoint>(Vector3(0.0f, 2.0f, 0.0f), 9, vertexActivityMask.test(9));
    std::shared_ptr<VertexPoint> node10 = std::make_shared<VertexPoint>(Vector3(2.0f, 2.0f, 0.0f), 10, vertexActivityMask.test(10));
    std::shared_ptr<VertexPoint> node11 = std::make_shared<VertexPoint>(Vector3(2.0f, 2.0f, 1.0f), 11, vertexActivityMask.test(11));
    std::shared_ptr<VertexPoint> node12 = std::make_shared<VertexPoint>(Vector3(1.0f, 2.0f, 1.0f), 12, vertexActivityMask.test(12));
    std::shared_ptr<VertexPoint> node13 = std::make_shared<VertexPoint>(Vector3(1.0f, 2.0f, 2.0f), 13, vertexActivityMask.test(13));
    std::shared_ptr<VertexPoint> node14 = std::make_shared<VertexPoint>(Vector3(0.0f, 2.0f, 2.0f), 14, vertexActivityMask.test(14));
    
    std::shared_ptr<DoupletVertexPoint> doupletNode15 = std::make_shared<DoupletVertexPoint>(15, node0, node1);
    std::shared_ptr<DoupletVertexPoint> doupletNode16 = std::make_shared<DoupletVertexPoint>(16, node0, node5);
    std::shared_ptr<DoupletVertexPoint> doupletNode17 = std::make_shared<DoupletVertexPoint>(17, node1, node10);
    std::shared_ptr<DoupletVertexPoint> doupletNode18 = std::make_shared<DoupletVertexPoint>(18, node0, node1, node9, node10);
    std::shared_ptr<DoupletVertexPoint> doupletNode19 = std::make_shared<DoupletVertexPoint>(19, node0, node5, node9, node14);
    std::shared_ptr<DoupletVertexPoint> doupletNode20 = std::make_shared<DoupletVertexPoint>(20, node5, node14);
    std::shared_ptr<DoupletVertexPoint> doupletNode21 = std::make_shared<DoupletVertexPoint>(21, node9, node10);
    std::shared_ptr<DoupletVertexPoint> doupletNode22 = std::make_shared<DoupletVertexPoint>(22, node9, node14);

    nodes = std::make_shared<std::vector<std::shared_ptr<VertexPoint>>>(15);
    doupletNodes = std::make_shared<std::vector<std::shared_ptr<DoupletVertexPoint>>>();

    doupletNodes->push_back(doupletNode15);
    doupletNodes->push_back(doupletNode16);
    doupletNodes->push_back(doupletNode17);
    doupletNodes->push_back(doupletNode18);
    doupletNodes->push_back(doupletNode19);
    doupletNodes->push_back(doupletNode20);
    doupletNodes->push_back(doupletNode21);
    doupletNodes->push_back(doupletNode22);

    node0->AddNeighbour(node1);
    node0->AddNeighbour(node5);
    node0->AddNeighbour(node9);

    node0->AddNeighbour(node3);
    
    AddNode(node0);

    node1->AddNeighbour(node0);
    node1->AddNeighbour(node2);
    node1->AddNeighbour(node10);

    AddNode(node1);

    node2->AddNeighbour(node1);
    node2->AddNeighbour(node3);
    node2->AddNeighbour(node6);

    AddNode(node2);

    node3->AddNeighbour(node2);
    node3->AddNeighbour(node4);
    node3->AddNeighbour(node7);

    node3->AddNeighbour(node0);

    node3->AddDoupletNeighbour(doupletNode15);
    doupletNode15->SetLinkedVertexPoint(node3);

    node3->AddDoupletNeighbour(doupletNode16);
    doupletNode16->SetLinkedVertexPoint(node3);

    AddNode(node3);

    node4->AddNeighbour(node3);
    node4->AddNeighbour(node5);
    node4->AddNeighbour(node8);

    AddNode(node4);

    node5->AddNeighbour(node0);
    node5->AddNeighbour(node4);
    node5->AddNeighbour(node14);

    AddNode(node5);

    node6->AddNeighbour(node2);
    node6->AddNeighbour(node7);
    node6->AddNeighbour(node11);

    node6->AddDoupletNeighbour(doupletNode17);
    doupletNode17->SetLinkedVertexPoint(node6);

    AddNode(node6);
    
    node7->AddNeighbour(node3);
    node7->AddNeighbour(node6);
    node7->AddNeighbour(node8);
    node7->AddNeighbour(node12);

    node7->AddDoupletNeighbour(doupletNode18);
    doupletNode18->SetLinkedVertexPoint(node7);

    node7->AddDoupletNeighbour(doupletNode19);
    doupletNode19->SetLinkedVertexPoint(node7);

    AddNode(node7);

    node8->AddNeighbour(node4);
    node8->AddNeighbour(node7);
    node8->AddNeighbour(node13);

    node8->AddDoupletNeighbour(doupletNode20);
    doupletNode20->SetLinkedVertexPoint(node8);
    
    AddNode(node8);

    node9->AddNeighbour(node0);
    node9->AddNeighbour(node10);
    node9->AddNeighbour(node14);

    node9->AddNeighbour(node12);

    AddNode(node9);

    node10->AddNeighbour(node1);
    node10->AddNeighbour(node9);
    node10->AddNeighbour(node11);

    AddNode(node10);

    node11->AddNeighbour(node6);
    node11->AddNeighbour(node10);
    node11->AddNeighbour(node12);

    AddNode(node11);

    node12->AddNeighbour(node7);
    node12->AddNeighbour(node11);
    node12->AddNeighbour(node13);

    node12->AddNeighbour(node9);

    node12->AddDoupletNeighbour(doupletNode21);
    doupletNode21->SetLinkedVertexPoint(node12);

    node12->AddDoupletNeighbour(doupletNode22);
    doupletNode22->SetLinkedVertexPoint(node12);

    AddNode(node12);

    node13->AddNeighbour(node8);
    node13->AddNeighbour(node12);
    node13->AddNeighbour(node14);

    AddNode(node13);

    node14->AddNeighbour(node5);
    node14->AddNeighbour(node9);
    node14->AddNeighbour(node13);

    AddNode(node14);

    if (edgesTable == nullptr)
    {
        edgesTable = std::make_unique<std::unordered_map<std::pair<int, int>, std::shared_ptr<EdgePoint>, Graph::pairHasher>>();

        std::shared_ptr<EdgePoint> edge0 = std::make_shared<EdgePoint>(*node0, *node1, 54);
        edgesTable->insert(std::make_pair(std::make_pair(edge0->GetFirstParentIndex(), edge0->GetSecondParentIndex()), edge0));

        std::shared_ptr<EdgePoint> edge1 = std::make_shared<EdgePoint>(*node1, *node2, 13);
        edgesTable->insert(std::make_pair(std::make_pair(edge1->GetFirstParentIndex(), edge1->GetSecondParentIndex()), edge1));

        std::shared_ptr<EdgePoint> edge2 = std::make_shared<EdgePoint>(*node2, *node3, 14);
        edgesTable->insert(std::make_pair(std::make_pair(edge2->GetFirstParentIndex(), edge2->GetSecondParentIndex()), edge2));

        std::shared_ptr<EdgePoint> edge3 = std::make_shared<EdgePoint>(*node3, *node4, 20);
        edgesTable->insert(std::make_pair(std::make_pair(edge3->GetFirstParentIndex(), edge3->GetSecondParentIndex()), edge3));

        std::shared_ptr<EdgePoint> edge4 = std::make_shared<EdgePoint>(*node4, *node5, 21);
        edgesTable->insert(std::make_pair(std::make_pair(edge4->GetFirstParentIndex(), edge4->GetSecondParentIndex()), edge4));

        std::shared_ptr<EdgePoint> edge5 = std::make_shared<EdgePoint>(*node0, *node5, 58);
        edgesTable->insert(std::make_pair(std::make_pair(edge5->GetFirstParentIndex(), edge5->GetSecondParentIndex()), edge5));

        std::shared_ptr<EdgePoint> edge6 = std::make_shared<EdgePoint>(*node2, *node6, 16);
        edgesTable->insert(std::make_pair(std::make_pair(edge6->GetFirstParentIndex(), edge6->GetSecondParentIndex()), edge6));

        std::shared_ptr<EdgePoint> edge7 = std::make_shared<EdgePoint>(*node3, *node7, 5);
        edgesTable->insert(std::make_pair(std::make_pair(edge7->GetFirstParentIndex(), edge7->GetSecondParentIndex()), edge7));

        std::shared_ptr<EdgePoint> edge8 = std::make_shared<EdgePoint>(*node4, *node8, 23);
        edgesTable->insert(std::make_pair(std::make_pair(edge8->GetFirstParentIndex(), edge8->GetSecondParentIndex()), edge8));

        std::shared_ptr<EdgePoint> edge9 = std::make_shared<EdgePoint>(*node1, *node10, 55);
        edgesTable->insert(std::make_pair(std::make_pair(edge9->GetFirstParentIndex(), edge9->GetSecondParentIndex()), edge9));

        std::shared_ptr<EdgePoint> edge10 = std::make_shared<EdgePoint>(*node0, *node9, 57);
        edgesTable->insert(std::make_pair(std::make_pair(edge10->GetFirstParentIndex(), edge10->GetSecondParentIndex()), edge10));

        std::shared_ptr<EdgePoint> edge11 = std::make_shared<EdgePoint>(*node6, *node7, 19);
        edgesTable->insert(std::make_pair(std::make_pair(edge11->GetFirstParentIndex(), edge11->GetSecondParentIndex()), edge11));

        std::shared_ptr<EdgePoint> edge12 = std::make_shared<EdgePoint>(*node7, *node8, 25);
        edgesTable->insert(std::make_pair(std::make_pair(edge12->GetFirstParentIndex(), edge12->GetSecondParentIndex()), edge12));

        std::shared_ptr<EdgePoint> edge13 = std::make_shared<EdgePoint>(*node5, *node14, 59);
        edgesTable->insert(std::make_pair(std::make_pair(edge13->GetFirstParentIndex(), edge13->GetSecondParentIndex()), edge13));

        std::shared_ptr<EdgePoint> edge14 = std::make_shared<EdgePoint>(*node9, *node10, 56);
        edgesTable->insert(std::make_pair(std::make_pair(edge14->GetFirstParentIndex(), edge14->GetSecondParentIndex()), edge14));

        std::shared_ptr<EdgePoint> edge15 = std::make_shared<EdgePoint>(*node10, *node11, 44);
        edgesTable->insert(std::make_pair(std::make_pair(edge15->GetFirstParentIndex(), edge15->GetSecondParentIndex()), edge15));

        std::shared_ptr<EdgePoint> edge16 = std::make_shared<EdgePoint>(*node6, *node11, 42);
        edgesTable->insert(std::make_pair(std::make_pair(edge16->GetFirstParentIndex(), edge16->GetSecondParentIndex()), edge16));

        std::shared_ptr<EdgePoint> edge17 = std::make_shared<EdgePoint>(*node7, *node12, 35);
        edgesTable->insert(std::make_pair(std::make_pair(edge17->GetFirstParentIndex(), edge17->GetSecondParentIndex()), edge17));

        std::shared_ptr<EdgePoint> edge18 = std::make_shared<EdgePoint>(*node8, *node13, 46);
        edgesTable->insert(std::make_pair(std::make_pair(edge18->GetFirstParentIndex(), edge18->GetSecondParentIndex()), edge18));

        std::shared_ptr<EdgePoint> edge19 = std::make_shared<EdgePoint>(*node11, *node12, 45);
        edgesTable->insert(std::make_pair(std::make_pair(edge19->GetFirstParentIndex(), edge19->GetSecondParentIndex()), edge19));
        
        std::shared_ptr<EdgePoint> edge20 = std::make_shared<EdgePoint>(*node12, *node13, 48);
        edgesTable->insert(std::make_pair(std::make_pair(edge20->GetFirstParentIndex(), edge20->GetSecondParentIndex()), edge20));

        std::shared_ptr<EdgePoint> edge21 = std::make_shared<EdgePoint>(*node13, *node14, 49);
        edgesTable->insert(std::make_pair(std::make_pair(edge21->GetFirstParentIndex(), edge21->GetSecondParentIndex()), edge21));

        std::shared_ptr<EdgePoint> edge22 = std::make_shared<EdgePoint>(*node3, *doupletNode15, 1);
        edgesTable->insert(std::make_pair(std::make_pair(edge22->GetFirstParentIndex(), edge22->GetSecondParentIndex()), edge22));

        std::shared_ptr<EdgePoint> edge23 = std::make_shared<EdgePoint>(*node3, *doupletNode16, 2);
        edgesTable->insert(std::make_pair(std::make_pair(edge23->GetFirstParentIndex(), edge23->GetSecondParentIndex()), edge23));

        std::shared_ptr<EdgePoint> edge24 = std::make_shared<EdgePoint>(*node6, *doupletNode17, 18);
        edgesTable->insert(std::make_pair(std::make_pair(edge24->GetFirstParentIndex(), edge24->GetSecondParentIndex()), edge24));

        std::shared_ptr<EdgePoint> edge25 = std::make_shared<EdgePoint>(*node7, *doupletNode18, 9);
        edgesTable->insert(std::make_pair(std::make_pair(edge25->GetFirstParentIndex(), edge25->GetSecondParentIndex()), edge25));

        std::shared_ptr<EdgePoint> edge26 = std::make_shared<EdgePoint>(*node7, *doupletNode19, 10);
        edgesTable->insert(std::make_pair(std::make_pair(edge26->GetFirstParentIndex(), edge26->GetSecondParentIndex()), edge26));

        std::shared_ptr<EdgePoint> edge27 = std::make_shared<EdgePoint>(*node8, *doupletNode20, 26);
        edgesTable->insert(std::make_pair(std::make_pair(edge27->GetFirstParentIndex(), edge27->GetSecondParentIndex()), edge27));

        std::shared_ptr<EdgePoint> edge28 = std::make_shared<EdgePoint>(*node12, *doupletNode21, 38);
        edgesTable->insert(std::make_pair(std::make_pair(edge28->GetFirstParentIndex(), edge28->GetSecondParentIndex()), edge28));

        std::shared_ptr<EdgePoint> edge29 = std::make_shared<EdgePoint>(*node12, *doupletNode22, 39);
        edgesTable->insert(std::make_pair(std::make_pair(edge29->GetFirstParentIndex(), edge29->GetSecondParentIndex()), edge29));

        std::shared_ptr<EdgePoint> edge30 = std::make_shared<EdgePoint>(*node9, *node14, 60);
        edgesTable->insert(std::make_pair(std::make_pair(edge30->GetFirstParentIndex(), edge30->GetSecondParentIndex()), edge30));
    }

    doupletNode15->SetLinkedEdgePoint(edgesTable->at(std::make_pair(3, 15)));
    doupletNode16->SetLinkedEdgePoint(edgesTable->at(std::make_pair(3, 16)));
    doupletNode17->SetLinkedEdgePoint(edgesTable->at(std::make_pair(6, 17)));
    doupletNode18->SetLinkedEdgePoint(edgesTable->at(std::make_pair(7, 18)));
    doupletNode19->SetLinkedEdgePoint(edgesTable->at(std::make_pair(7, 19)));
    doupletNode20->SetLinkedEdgePoint(edgesTable->at(std::make_pair(8, 20)));
    doupletNode21->SetLinkedEdgePoint(edgesTable->at(std::make_pair(12, 21)));
    doupletNode22->SetLinkedEdgePoint(edgesTable->at(std::make_pair(12, 22)));

    if (edgeToFaceTable == nullptr)
    {
        edgeToFaceTable = std::make_unique<std::unordered_map<int, std::unordered_set<int>>>();

        edgeToFaceTable->insert(std::make_pair(1, std::unordered_set<int>{8, 9, 16, 17}));
        edgeToFaceTable->insert(std::make_pair(2, std::unordered_set<int>{8, 13, 25, 31}));
        edgeToFaceTable->insert(std::make_pair(5, std::unordered_set<int>{0, 1, 17, 18, 31, 32}));
        edgeToFaceTable->insert(std::make_pair(9, std::unordered_set<int>{16, 18, 19, 20, 21,22, 23, 33, 34,37, 38}));
        edgeToFaceTable->insert(std::make_pair(10, std::unordered_set<int>{20, 25, 26, 27, 28, 29, 32, 35, 36, 39, 40}));
        edgeToFaceTable->insert(std::make_pair(13, std::unordered_set<int>{4, 9, 33}));
        edgeToFaceTable->insert(std::make_pair(14, std::unordered_set<int>{1, 9}));
        edgeToFaceTable->insert(std::make_pair(16, std::unordered_set<int>{1, 4}));
        edgeToFaceTable->insert(std::make_pair(18, std::unordered_set<int>{4, 5,22}));
        edgeToFaceTable->insert(std::make_pair(19, std::unordered_set<int>{1, 3, 19}));
        edgeToFaceTable->insert(std::make_pair(20, std::unordered_set<int>{0, 13}));
        edgeToFaceTable->insert(std::make_pair(21, std::unordered_set<int>{13, 14, 35}));
        edgeToFaceTable->insert(std::make_pair(23, std::unordered_set<int>{0, 14}));
        edgeToFaceTable->insert(std::make_pair(25, std::unordered_set<int>{0, 2, 28}));
        edgeToFaceTable->insert(std::make_pair(26, std::unordered_set<int>{14, 15, 27}));
        edgeToFaceTable->insert(std::make_pair(35, std::unordered_set<int>{2, 3, 21, 24, 29, 30}));
        edgeToFaceTable->insert(std::make_pair(38, std::unordered_set<int>{6, 7, 23, 24}));
        edgeToFaceTable->insert(std::make_pair(39, std::unordered_set<int>{6, 11, 26, 30}));
        edgeToFaceTable->insert(std::make_pair(42, std::unordered_set<int>{3, 5}));
        edgeToFaceTable->insert(std::make_pair(44, std::unordered_set<int>{5, 7, 34}));      
        edgeToFaceTable->insert(std::make_pair(45, std::unordered_set<int>{3, 7}));
        edgeToFaceTable->insert(std::make_pair(46, std::unordered_set<int>{2, 15}));     
        edgeToFaceTable->insert(std::make_pair(48, std::unordered_set<int>{2, 11}));
        edgeToFaceTable->insert(std::make_pair(49, std::unordered_set<int>{11, 15, 36}));
        edgeToFaceTable->insert(std::make_pair(54, std::unordered_set<int>{8, 9, 10, 37}));      
        edgeToFaceTable->insert(std::make_pair(55, std::unordered_set<int>{4, 5, 10}));
        edgeToFaceTable->insert(std::make_pair(56, std::unordered_set<int>{6, 7, 10, 38}));
        edgeToFaceTable->insert(std::make_pair(57, std::unordered_set<int>{10, 12}));
        edgeToFaceTable->insert(std::make_pair(58, std::unordered_set<int>{8, 12, 13, 39}));
        edgeToFaceTable->insert(std::make_pair(59, std::unordered_set<int>{12, 14, 15}));
        edgeToFaceTable->insert(std::make_pair(60, std::unordered_set<int>{6, 11, 12, 40}));
    }

    prohibitedAreaBoundingBox = AABB(Vector3(0.0f,0.0f,0.5), Vector3(1.0f, 0.5f, 1.0f));
}

bool Graph17::IsProhibited(int edge1Index, int edge2Index) const
{
    if (edge1Index > edge2Index)
    {
        std::swap(edge1Index, edge2Index);
    }
    if (edge1Index == 14 && edge2Index == 20)
    {
        return true;
    }
    if (edge1Index == 19 && edge2Index == 25)
    {
        return true;
    }
    if (edge1Index == 45 && edge2Index == 48)
    {
        return true;
    }
    if (edge1Index == 1 && edge2Index == 20)
    {
        return true;
    }
    if (edge1Index == 2 && edge2Index == 14)
    {
        return true;
    }
    if (edge1Index == 10 && edge2Index == 19)
    {
        return true;
    }
    if (edge1Index == 9 && edge2Index == 25)
    {
        return true;
    }
    if (edge1Index == 38 && edge2Index == 48)
    {
        return true;
    }
    if (edge1Index == 39 && edge2Index == 45)
    {
        return true;
    }
    return false;
}

void Graph17::RemoveRedundantLinks(EdgePointsGraph* edgePointsGraph)
{
    int edgeIndex = 54;
    bool found = false;
    for (auto it : edgePointsGraph->nodes)
    {
        if (it->GetIndex() == edgeIndex)
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        for (int i = edgePointsGraph->nodes.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->nodes[i]->GetIndex() == 1)
            {
                edgePointsGraph->nodes.erase(edgePointsGraph->nodes.begin() + i);
            }
        }
        for (int i = edgePointsGraph->links.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->links[i][0]->GetIndex() == 1 || edgePointsGraph->links[i][1]->GetIndex() == 1)
            {
                edgePointsGraph->links.erase(edgePointsGraph->links.begin() + i);
            }
        }
    }

    edgeIndex = 55;
    found = false;
    for (auto it : edgePointsGraph->nodes)
    {
        if (it->GetIndex() == edgeIndex)
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        for (int i = edgePointsGraph->nodes.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->nodes[i]->GetIndex() == 18)
            {
                edgePointsGraph->nodes.erase(edgePointsGraph->nodes.begin() + i);
            }
        }
        for (int i = edgePointsGraph->links.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->links[i][0]->GetIndex() == 18 || edgePointsGraph->links[i][1]->GetIndex() == 18)
            {
                edgePointsGraph->links.erase(edgePointsGraph->links.begin() + i);
            }
        }
    }

    edgeIndex = 56;
    found = false;
    for (auto it : edgePointsGraph->nodes)
    {
        if (it->GetIndex() == edgeIndex)
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        for (int i = edgePointsGraph->nodes.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->nodes[i]->GetIndex() == 38)
            {
                edgePointsGraph->nodes.erase(edgePointsGraph->nodes.begin() + i);
            }
        }
        for (int i = edgePointsGraph->links.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->links[i][0]->GetIndex() == 38 || edgePointsGraph->links[i][1]->GetIndex() == 38)
            {
                edgePointsGraph->links.erase(edgePointsGraph->links.begin() + i);
            }
        }
    }

    edgeIndex = 58;
    found = false;
    for (auto it : edgePointsGraph->nodes)
    {
        if (it->GetIndex() == edgeIndex)
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        for (int i = edgePointsGraph->nodes.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->nodes[i]->GetIndex() == 2)
            {
                edgePointsGraph->nodes.erase(edgePointsGraph->nodes.begin() + i);
            }
        }
        for (int i = edgePointsGraph->links.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->links[i][0]->GetIndex() == 2 || edgePointsGraph->links[i][1]->GetIndex() == 2)
            {
                edgePointsGraph->links.erase(edgePointsGraph->links.begin() + i);
            }
        }
    }

    edgeIndex = 59;
    found = false;
    for (auto it : edgePointsGraph->nodes)
    {
        if (it->GetIndex() == edgeIndex)
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        for (int i = edgePointsGraph->nodes.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->nodes[i]->GetIndex() == 26)
            {
                edgePointsGraph->nodes.erase(edgePointsGraph->nodes.begin() + i);
            }
        }
        for (int i = edgePointsGraph->links.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->links[i][0]->GetIndex() == 26 || edgePointsGraph->links[i][1]->GetIndex() == 26)
            {
                edgePointsGraph->links.erase(edgePointsGraph->links.begin() + i);
            }
        }
    }

    edgeIndex = 60;
    found = false;
    for (auto it : edgePointsGraph->nodes)
    {
        if (it->GetIndex() == edgeIndex)
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        for (int i = edgePointsGraph->nodes.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->nodes[i]->GetIndex() == 39)
            {
                edgePointsGraph->nodes.erase(edgePointsGraph->nodes.begin() + i);
            }
        }
        for (int i = edgePointsGraph->links.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->links[i][0]->GetIndex() == 39 || edgePointsGraph->links[i][1]->GetIndex() == 39)
            {
                edgePointsGraph->links.erase(edgePointsGraph->links.begin() + i);
            }
        }
    }
}