#include "Graph20.hpp"
#include "../types.hpp"
#include "../Points/VertexPoints/VertexPoint.hpp"

Graph20::Graph20()
{
}

void Graph20::Create(const VertexActivityMask& vertexActivityMask)
{
    std::shared_ptr<VertexPoint> node0 = std::make_shared<VertexPoint>(Vector3(0.0f, 0.0f, 0.0f), 0, vertexActivityMask.test(0));
    std::shared_ptr<VertexPoint> node1 = std::make_shared<VertexPoint>(Vector3(2.0f, 0.0f, 0.0f), 1, vertexActivityMask.test(1));
    std::shared_ptr<VertexPoint> node2 = std::make_shared<VertexPoint>(Vector3(2.0f, 0.0f, 2.0f), 2, vertexActivityMask.test(2));
    std::shared_ptr<VertexPoint> node3 = std::make_shared<VertexPoint>(Vector3(0.0f, 0.0f, 2.0f), 3, vertexActivityMask.test(3));
    std::shared_ptr<VertexPoint> node4 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 1.0f), 4, vertexActivityMask.test(4));
    std::shared_ptr<VertexPoint> node5 = std::make_shared<VertexPoint>(Vector3(2.0f, 1.0f, 1.0f), 5, vertexActivityMask.test(5));
    std::shared_ptr<VertexPoint> node6 = std::make_shared<VertexPoint>(Vector3(2.0f, 1.0f, 2.0f), 6, vertexActivityMask.test(6));
    std::shared_ptr<VertexPoint> node7 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 2.0f), 7, vertexActivityMask.test(7));
    std::shared_ptr<VertexPoint> node8 = std::make_shared<VertexPoint>(Vector3(0.0f, 2.0f, 0.0f), 8, vertexActivityMask.test(8));
    std::shared_ptr<VertexPoint> node9 = std::make_shared<VertexPoint>(Vector3(2.0f, 2.0f, 0.0f), 9, vertexActivityMask.test(9));
    std::shared_ptr<VertexPoint> node10 = std::make_shared<VertexPoint>(Vector3(2.0f, 2.0f, 1.0f), 10, vertexActivityMask.test(10));
    std::shared_ptr<VertexPoint> node11 = std::make_shared<VertexPoint>(Vector3(1.0f, 2.0f, 1.0f), 11, vertexActivityMask.test(11));
    std::shared_ptr<VertexPoint> node12 = std::make_shared<VertexPoint>(Vector3(1.0f, 2.0f, 2.0f), 12, vertexActivityMask.test(12));
    std::shared_ptr<VertexPoint> node13 = std::make_shared<VertexPoint>(Vector3(0.0f, 2.0f, 2.0f), 13, vertexActivityMask.test(13));

    std::shared_ptr<DoupletVertexPoint> doupletNode14 = std::make_shared<DoupletVertexPoint>(14, node0, node1, node8, node9);
    std::shared_ptr<DoupletVertexPoint> doupletNode15 = std::make_shared<DoupletVertexPoint>(15, node1, node9);
    std::shared_ptr<DoupletVertexPoint> doupletNode16 = std::make_shared<DoupletVertexPoint>(16, node8, node9);
    std::shared_ptr<DoupletVertexPoint> doupletNode17 = std::make_shared<DoupletVertexPoint>(17, node0, node3, node8, node13);
    std::shared_ptr<DoupletVertexPoint> doupletNode18 = std::make_shared<DoupletVertexPoint>(18, node3, node13);
    std::shared_ptr<DoupletVertexPoint> doupletNode19 = std::make_shared<DoupletVertexPoint>(19, node8, node13);
    std::shared_ptr<DoupletVertexPoint> doupletNode20 = std::make_shared<DoupletVertexPoint>(20, node0, node1, node2, node3);
    std::shared_ptr<DoupletVertexPoint> doupletNode21 = std::make_shared<DoupletVertexPoint>(21, node1, node2);
    std::shared_ptr<DoupletVertexPoint> doupletNode22 = std::make_shared<DoupletVertexPoint>(22, node2, node3);

    nodes = std::make_shared<std::vector<std::shared_ptr<VertexPoint>>>(14);
    doupletNodes = std::make_shared<std::vector<std::shared_ptr<DoupletVertexPoint>>>();

    doupletNodes->push_back(doupletNode14);
    doupletNodes->push_back(doupletNode15);
    doupletNodes->push_back(doupletNode16);
    doupletNodes->push_back(doupletNode17);
    doupletNodes->push_back(doupletNode18);
    doupletNodes->push_back(doupletNode19);
    doupletNodes->push_back(doupletNode20);
    doupletNodes->push_back(doupletNode21);
    doupletNodes->push_back(doupletNode22);

    node0->AddNeighbour(node1);
    node0->AddNeighbour(node3);
    node0->AddNeighbour(node8);

    AddNode(node0);

    node1->AddNeighbour(node0);
    node1->AddNeighbour(node2);
    node1->AddNeighbour(node9);

    AddNode(node1);

    node2->AddNeighbour(node1);
    node2->AddNeighbour(node3);
    node2->AddNeighbour(node6);

    AddNode(node2);

    node3->AddNeighbour(node0);
    node3->AddNeighbour(node2);
    node3->AddNeighbour(node13);

    AddNode(node3);

    node4->AddNeighbour(node5);
    node4->AddNeighbour(node7);
    node4->AddNeighbour(node11);

    node4->AddDoupletNeighbour(doupletNode14);
    doupletNode14->SetLinkedVertexPoint(node4);

    node4->AddDoupletNeighbour(doupletNode17);
    doupletNode17->SetLinkedVertexPoint(node4);

    node4->AddDoupletNeighbour(doupletNode20);
    doupletNode20->SetLinkedVertexPoint(node4);

    AddNode(node4);

    node5->AddNeighbour(node4);
    node5->AddNeighbour(node6);
    node5->AddNeighbour(node10);

    node5->AddDoupletNeighbour(doupletNode15);
    doupletNode15->SetLinkedVertexPoint(node6);

    node5->AddDoupletNeighbour(doupletNode21);
    doupletNode21->SetLinkedVertexPoint(node6);

    AddNode(node5);

    node6->AddNeighbour(node2);
    node6->AddNeighbour(node5);
    node6->AddNeighbour(node7);

    AddNode(node6);

    node7->AddNeighbour(node4);
    node7->AddNeighbour(node6);
    node7->AddNeighbour(node12);

    node7->AddDoupletNeighbour(doupletNode18);
    doupletNode18->SetLinkedVertexPoint(node7);

    node7->AddDoupletNeighbour(doupletNode22);
    doupletNode22->SetLinkedVertexPoint(node7);

    AddNode(node7);

    node8->AddNeighbour(node0);
    node8->AddNeighbour(node9);
    node8->AddNeighbour(node13);

    AddNode(node8);

    node9->AddNeighbour(node1);
    node9->AddNeighbour(node8);
    node9->AddNeighbour(node10);

    node9->AddNeighbour(node12);

    AddNode(node9);

    node10->AddNeighbour(node5);
    node10->AddNeighbour(node9);
    node10->AddNeighbour(node11);

    AddNode(node10);

    node11->AddNeighbour(node4);
    node11->AddNeighbour(node10);
    node11->AddNeighbour(node12);

    node11->AddDoupletNeighbour(doupletNode16);
    doupletNode16->SetLinkedVertexPoint(node11);

    node11->AddDoupletNeighbour(doupletNode19);
    doupletNode19->SetLinkedVertexPoint(node11);

    AddNode(node11);

    node12->AddNeighbour(node7);
    node12->AddNeighbour(node11);
    node12->AddNeighbour(node13);

    AddNode(node12);

    node13->AddNeighbour(node8);
    node13->AddNeighbour(node3);
    node13->AddNeighbour(node12);

    AddNode(node13);

    if (edgesTable == nullptr)
    {
        edgesTable = std::make_unique<std::unordered_map<std::pair<int, int>, std::shared_ptr<EdgePoint>, Graph::pairHasher>>();

        std::shared_ptr<EdgePoint> edge0 = std::make_shared<EdgePoint>(node0, node1, 54);
        edgesTable->insert(std::make_pair(std::make_pair(edge0->GetFirstParentIndex(), edge0->GetSecondParentIndex()), edge0));

        std::shared_ptr<EdgePoint> edge1 = std::make_shared<EdgePoint>(node1, node2, 63);
        edgesTable->insert(std::make_pair(std::make_pair(edge1->GetFirstParentIndex(), edge1->GetSecondParentIndex()), edge1));

        std::shared_ptr<EdgePoint> edge2 = std::make_shared<EdgePoint>(node2, node3, 64);
        edgesTable->insert(std::make_pair(std::make_pair(edge2->GetFirstParentIndex(), edge2->GetSecondParentIndex()), edge2));

        std::shared_ptr<EdgePoint> edge3 = std::make_shared<EdgePoint>(node0, node3, 58);
        edgesTable->insert(std::make_pair(std::make_pair(edge3->GetFirstParentIndex(), edge3->GetSecondParentIndex()), edge3));

        std::shared_ptr<EdgePoint> edge4 = std::make_shared<EdgePoint>(node4, node5, 19);
        edgesTable->insert(std::make_pair(std::make_pair(edge4->GetFirstParentIndex(), edge4->GetSecondParentIndex()), edge4));

        std::shared_ptr<EdgePoint> edge5 = std::make_shared<EdgePoint>(node0, node8, 57);
        edgesTable->insert(std::make_pair(std::make_pair(edge5->GetFirstParentIndex(), edge5->GetSecondParentIndex()), edge5));

        std::shared_ptr<EdgePoint> edge6 = std::make_shared<EdgePoint>(node1, node9, 55);
        edgesTable->insert(std::make_pair(std::make_pair(edge6->GetFirstParentIndex(), edge6->GetSecondParentIndex()), edge6));

        std::shared_ptr<EdgePoint> edge7 = std::make_shared<EdgePoint>(node8, node9, 56);
        edgesTable->insert(std::make_pair(std::make_pair(edge7->GetFirstParentIndex(), edge7->GetSecondParentIndex()), edge7));

        std::shared_ptr<EdgePoint> edge8 = std::make_shared<EdgePoint>(node8, node13, 60);
        edgesTable->insert(std::make_pair(std::make_pair(edge8->GetFirstParentIndex(), edge8->GetSecondParentIndex()), edge8));

        std::shared_ptr<EdgePoint> edge9 = std::make_shared<EdgePoint>(node9, node10, 44);
        edgesTable->insert(std::make_pair(std::make_pair(edge9->GetFirstParentIndex(), edge9->GetSecondParentIndex()), edge9));

        std::shared_ptr<EdgePoint> edge10 = std::make_shared<EdgePoint>(node10, node11, 45);
        edgesTable->insert(std::make_pair(std::make_pair(edge10->GetFirstParentIndex(), edge10->GetSecondParentIndex()), edge10));

        std::shared_ptr<EdgePoint> edge11 = std::make_shared<EdgePoint>(node11, node12, 48);
        edgesTable->insert(std::make_pair(std::make_pair(edge11->GetFirstParentIndex(), edge11->GetSecondParentIndex()), edge11));

        std::shared_ptr<EdgePoint> edge12 = std::make_shared<EdgePoint>(node12, node13, 49);
        edgesTable->insert(std::make_pair(std::make_pair(edge12->GetFirstParentIndex(), edge12->GetSecondParentIndex()), edge12));

        std::shared_ptr<EdgePoint> edge13 = std::make_shared<EdgePoint>(node5, node10, 42);
        edgesTable->insert(std::make_pair(std::make_pair(edge13->GetFirstParentIndex(), edge13->GetSecondParentIndex()), edge13));

        std::shared_ptr<EdgePoint> edge14 = std::make_shared<EdgePoint>(node4, node11, 35);
        edgesTable->insert(std::make_pair(std::make_pair(edge14->GetFirstParentIndex(), edge14->GetSecondParentIndex()), edge14));

        std::shared_ptr<EdgePoint> edge15 = std::make_shared<EdgePoint>(node4, node7, 25);
        edgesTable->insert(std::make_pair(std::make_pair(edge15->GetFirstParentIndex(), edge15->GetSecondParentIndex()), edge15));

        std::shared_ptr<EdgePoint> edge16 = std::make_shared<EdgePoint>(node3, node13, 59);
        edgesTable->insert(std::make_pair(std::make_pair(edge16->GetFirstParentIndex(), edge16->GetSecondParentIndex()), edge16));

        std::shared_ptr<EdgePoint> edge17 = std::make_shared<EdgePoint>(node6, node7, 32);
        edgesTable->insert(std::make_pair(std::make_pair(edge17->GetFirstParentIndex(), edge17->GetSecondParentIndex()), edge17));

        std::shared_ptr<EdgePoint> edge18 = std::make_shared<EdgePoint>(node5, node6, 31);
        edgesTable->insert(std::make_pair(std::make_pair(edge18->GetFirstParentIndex(), edge18->GetSecondParentIndex()), edge18));

        std::shared_ptr<EdgePoint> edge19 = std::make_shared<EdgePoint>(node2, node6, 30);
        edgesTable->insert(std::make_pair(std::make_pair(edge19->GetFirstParentIndex(), edge19->GetSecondParentIndex()), edge19));

        std::shared_ptr<EdgePoint> edge20 = std::make_shared<EdgePoint>(node7, node12, 46);
        edgesTable->insert(std::make_pair(std::make_pair(edge20->GetFirstParentIndex(), edge20->GetSecondParentIndex()), edge20));

        std::shared_ptr<EdgePoint> edge21 = std::make_shared<EdgePoint>(node4, doupletNode14, 9);
        edgesTable->insert(std::make_pair(std::make_pair(edge21->GetFirstParentIndex(), edge21->GetSecondParentIndex()), edge21));

        std::shared_ptr<EdgePoint> edge22 = std::make_shared<EdgePoint>(node5, doupletNode15, 18);
        edgesTable->insert(std::make_pair(std::make_pair(edge22->GetFirstParentIndex(), edge22->GetSecondParentIndex()), edge22));

        std::shared_ptr<EdgePoint> edge23 = std::make_shared<EdgePoint>(node11, doupletNode16, 38);
        edgesTable->insert(std::make_pair(std::make_pair(edge23->GetFirstParentIndex(), edge23->GetSecondParentIndex()), edge23));

        std::shared_ptr<EdgePoint> edge24 = std::make_shared<EdgePoint>(node4, doupletNode17, 10);
        edgesTable->insert(std::make_pair(std::make_pair(edge24->GetFirstParentIndex(), edge24->GetSecondParentIndex()), edge24));

        std::shared_ptr<EdgePoint> edge25 = std::make_shared<EdgePoint>(node7, doupletNode18, 26);
        edgesTable->insert(std::make_pair(std::make_pair(edge25->GetFirstParentIndex(), edge25->GetSecondParentIndex()), edge25));

        std::shared_ptr<EdgePoint> edge26 = std::make_shared<EdgePoint>(node11, doupletNode19, 39);
        edgesTable->insert(std::make_pair(std::make_pair(edge26->GetFirstParentIndex(), edge26->GetSecondParentIndex()), edge26));

        std::shared_ptr<EdgePoint> edge27 = std::make_shared<EdgePoint>(node4, doupletNode20, 5);
        edgesTable->insert(std::make_pair(std::make_pair(edge27->GetFirstParentIndex(), edge27->GetSecondParentIndex()), edge27));

        std::shared_ptr<EdgePoint> edge28 = std::make_shared<EdgePoint>(node5, doupletNode21, 16);
        edgesTable->insert(std::make_pair(std::make_pair(edge28->GetFirstParentIndex(), edge28->GetSecondParentIndex()), edge28));

        std::shared_ptr<EdgePoint> edge29 = std::make_shared<EdgePoint>(node7, doupletNode22, 23);
        edgesTable->insert(std::make_pair(std::make_pair(edge29->GetFirstParentIndex(), edge29->GetSecondParentIndex()), edge29));
    }

    doupletNode14->SetLinkedEdgePoint(edgesTable->at(std::make_pair(4, 14)));
    doupletNode15->SetLinkedEdgePoint(edgesTable->at(std::make_pair(5, 15)));
    doupletNode16->SetLinkedEdgePoint(edgesTable->at(std::make_pair(11, 16)));
    doupletNode17->SetLinkedEdgePoint(edgesTable->at(std::make_pair(4, 17)));
    doupletNode18->SetLinkedEdgePoint(edgesTable->at(std::make_pair(7, 18)));
    doupletNode19->SetLinkedEdgePoint(edgesTable->at(std::make_pair(11, 19)));
    doupletNode20->SetLinkedEdgePoint(edgesTable->at(std::make_pair(4, 20)));
    doupletNode21->SetLinkedEdgePoint(edgesTable->at(std::make_pair(5, 21)));
    doupletNode22->SetLinkedEdgePoint(edgesTable->at(std::make_pair(7, 22)));

    if (edgeToFaceTable == nullptr)
    {
        edgeToFaceTable = std::make_unique<std::unordered_map<int, std::unordered_set<int>>>();

        edgeToFaceTable->insert(std::make_pair(5, std::unordered_set<int>{18, 30, 37, 39, 41, 42, 43, 44, 47, 49}));
        edgeToFaceTable->insert(std::make_pair(9, std::unordered_set<int>{18, 19, 20, 21, 22, 23, 31, 33, 34, 38, 40}));
        edgeToFaceTable->insert(std::make_pair(10, std::unordered_set<int>{20, 25, 26, 27, 28, 30, 32, 35, 36, 45, 48}));
        edgeToFaceTable->insert(std::make_pair(16, std::unordered_set<int>{2, 12, 39, 50}));
        edgeToFaceTable->insert(std::make_pair(18, std::unordered_set<int>{2, 3, 22}));
        edgeToFaceTable->insert(std::make_pair(19, std::unordered_set<int>{1, 14, 19, 49, 50}));
        edgeToFaceTable->insert(std::make_pair(23, std::unordered_set<int>{10, 13, 43, 46}));
        edgeToFaceTable->insert(std::make_pair(25, std::unordered_set<int>{0, 14, 27, 46, 47}));
        edgeToFaceTable->insert(std::make_pair(26, std::unordered_set<int>{10, 11, 26}));
        edgeToFaceTable->insert(std::make_pair(30, std::unordered_set<int>{12, 13}));
        edgeToFaceTable->insert(std::make_pair(31, std::unordered_set<int>{12, 14}));
        edgeToFaceTable->insert(std::make_pair(32, std::unordered_set<int>{13, 14}));
        edgeToFaceTable->insert(std::make_pair(35, std::unordered_set<int>{0, 1, 21, 24, 28, 29}));
        edgeToFaceTable->insert(std::make_pair(38, std::unordered_set<int>{4, 5, 23, 24}));
        edgeToFaceTable->insert(std::make_pair(39, std::unordered_set<int>{4, 8, 25, 29}));
        edgeToFaceTable->insert(std::make_pair(42, std::unordered_set<int>{1, 3}));
        edgeToFaceTable->insert(std::make_pair(44, std::unordered_set<int>{3, 5, 31}));
        edgeToFaceTable->insert(std::make_pair(45, std::unordered_set<int>{1, 5}));
        edgeToFaceTable->insert(std::make_pair(46, std::unordered_set<int>{0, 11}));
        edgeToFaceTable->insert(std::make_pair(48, std::unordered_set<int>{0, 8}));
        edgeToFaceTable->insert(std::make_pair(49, std::unordered_set<int>{8, 11, 32}));
        edgeToFaceTable->insert(std::make_pair(54, std::unordered_set<int>{6, 7, 33, 41}));
        edgeToFaceTable->insert(std::make_pair(55, std::unordered_set<int>{2, 3, 7, 40}));
        edgeToFaceTable->insert(std::make_pair(56, std::unordered_set<int>{4, 5, 7, 34}));
        edgeToFaceTable->insert(std::make_pair(57, std::unordered_set<int>{7, 9, 38, 48}));
        edgeToFaceTable->insert(std::make_pair(58, std::unordered_set<int>{6, 9, 35, 44}));
        edgeToFaceTable->insert(std::make_pair(59, std::unordered_set<int>{9, 10, 11, 45}));
        edgeToFaceTable->insert(std::make_pair(60, std::unordered_set<int>{4, 8, 9, 36}));
        edgeToFaceTable->insert(std::make_pair(61, std::unordered_set<int>{1, 3, 5}));
        edgeToFaceTable->insert(std::make_pair(62, std::unordered_set<int>{0, 8, 11}));
        edgeToFaceTable->insert(std::make_pair(63, std::unordered_set<int>{2, 6, 12, 42}));
        edgeToFaceTable->insert(std::make_pair(64, std::unordered_set<int>{6, 10, 13, 37}));
    }
}

bool Graph20::IsProhibited(int edge1Index, int edge2Index) const
{
    if (edge1Index > edge2Index)
    {
        std::swap(edge1Index, edge2Index);
    }
    if (edge1Index == 18 && edge2Index == 31)
    {
        return true;
    }
    if (edge1Index == 16 && edge2Index == 42)
    {
        return true;
    }
    if (edge1Index == 19 && edge2Index == 25)
    {
        return true;
    }
    if (edge1Index == 23 && edge2Index == 46)
    {
        return true;
    }
    if (edge1Index == 26 && edge2Index == 32)
    {
        return true;
    }
    if (edge1Index == 5 && edge2Index == 35)
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

void Graph20::RemoveRedundantLinks(EdgePointsGraph* edgePointsGraph)
{
    int edgeIndex = 55;
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

    edgeIndex = 63;
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
            if (edgePointsGraph->nodes[i]->GetIndex() == 16)
            {
                edgePointsGraph->nodes.erase(edgePointsGraph->nodes.begin() + i);
            }
        }
        for (int i = edgePointsGraph->links.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->links[i][0]->GetIndex() == 16 || edgePointsGraph->links[i][1]->GetIndex() == 16)
            {
                edgePointsGraph->links.erase(edgePointsGraph->links.begin() + i);
            }
        }
    }

    edgeIndex = 64;
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
            if (edgePointsGraph->nodes[i]->GetIndex() == 23)
            {
                edgePointsGraph->nodes.erase(edgePointsGraph->nodes.begin() + i);
            }
        }
        for (int i = edgePointsGraph->links.size() - 1; i >= 0; --i)
        {
            if (edgePointsGraph->links[i][0]->GetIndex() == 23 || edgePointsGraph->links[i][1]->GetIndex() == 23)
            {
                edgePointsGraph->links.erase(edgePointsGraph->links.begin() + i);
            }
        }
    }
}
