#include "Graph.hpp"
#include <iostream>
#include <memory>

std::unique_ptr<std::unordered_map<int, std::unordered_set<int>>> Graph::edgeToFaceTable = nullptr;
std::unique_ptr<std::unordered_map<std::pair<int, int>, std::shared_ptr<EdgePoint>, Graph::pairHasher>> Graph::edgesTable = nullptr;

Graph::Graph(const VertexActivityMask& vertexActivityMask)
{
    std::shared_ptr<VertexPoint> node0 = std::make_shared<VertexPoint>(Vector3(0.0f, 0.0f, 0.0f), 0, vertexActivityMask.test(0));
    std::shared_ptr<VertexPoint> node1 = std::make_shared<VertexPoint>(Vector3(0.5f, 0.0f, 0.0f), 1, vertexActivityMask.test(1));
    std::shared_ptr<VertexPoint> node2 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.0f, 0.0f), 2, vertexActivityMask.test(2));
    std::shared_ptr<VertexPoint> node3 = std::make_shared<VertexPoint>(Vector3(0.0f, 0.0f, 0.5f), 3, vertexActivityMask.test(3));
    std::shared_ptr<VertexPoint> node4 = std::make_shared<VertexPoint>(Vector3(0.5f, 0.0f, 0.5f), 4, vertexActivityMask.test(4));
    std::shared_ptr<VertexPoint> node5 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.0f, 0.5f), 5, vertexActivityMask.test(5));
    std::shared_ptr<VertexPoint> node6 = std::make_shared<VertexPoint>(Vector3(0.0f, 0.0f, 1.0f), 6, vertexActivityMask.test(6));
    std::shared_ptr<VertexPoint> node7 = std::make_shared<VertexPoint>(Vector3(0.5f, 0.0f, 1.0f), 7, vertexActivityMask.test(7));
    std::shared_ptr<VertexPoint> node8 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.0f, 1.0f), 8, vertexActivityMask.test(8));
    std::shared_ptr<VertexPoint> node9 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.5f, 0.0f), 9, vertexActivityMask.test(9));
    std::shared_ptr<VertexPoint> node10 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 0.0f), 10, vertexActivityMask.test(10));
    std::shared_ptr<VertexPoint> node11 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.5f, 0.5f), 11, vertexActivityMask.test(11));
    std::shared_ptr<VertexPoint> node12 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 0.5f), 12, vertexActivityMask.test(12));
    std::shared_ptr<VertexPoint> node13 = std::make_shared<VertexPoint>(Vector3(1.0f, 0.5f, 1.0f), 13, vertexActivityMask.test(13));
    std::shared_ptr<VertexPoint> node14 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 1.0f), 14, vertexActivityMask.test(14));
    std::shared_ptr<VertexPoint> node15 = std::make_shared<VertexPoint>(Vector3(0.0f, 0.5f, 0.0f), 15, vertexActivityMask.test(15));
    std::shared_ptr<VertexPoint> node16 = std::make_shared<VertexPoint>(Vector3(0.5f, 0.5f, 0.0f), 16, vertexActivityMask.test(16));
    std::shared_ptr<VertexPoint> node17 = std::make_shared<VertexPoint>(Vector3(0.0f, 1.0f, 0.0f), 17, vertexActivityMask.test(17));
    std::shared_ptr<VertexPoint> node18 = std::make_shared<VertexPoint>(Vector3(0.5f, 1.0f, 0.0f), 18, vertexActivityMask.test(18));
    std::shared_ptr<VertexPoint> node19 = std::make_shared<VertexPoint>(Vector3(0.0f, 1.0f, 1.0f), 19, vertexActivityMask.test(19));

    nodes = std::make_shared<std::array<std::shared_ptr<VertexPoint>, 20>>();

    if (node1->IsActive())
    {
        node0->AddNeighbour(node1);
    }
    else
    {
        node0->AddNeighbour(node2);
    }
    if (node3->IsActive())
    {
        node0->AddNeighbour(node3);
    }
    else
    {
        node0->AddNeighbour(node6);
    }
    if (node15->IsActive())
    {
        node0->AddNeighbour(node15);
    }
    else
    {
        node0->AddNeighbour(node17);
    }

    AddNode(node0);

    if (node1->IsActive())
    {
        node1->AddNeighbour(node0);
        node1->AddNeighbour(node2);
        node1->AddNeighbour(node4);
        node1->AddNeighbour(node16);

        AddNode(node1);
    }

    if (node1->IsActive())
    {
        node2->AddNeighbour(node1);
    }
    else
    {
        node2->AddNeighbour(node0);
    }
    if (node5->IsActive())
    {
        node2->AddNeighbour(node5);
    }
    else
    {
        node2->AddNeighbour(node8);
    }
    if (node9->IsActive())
    {
        node2->AddNeighbour(node9);
    }
    else
    {
        node2->AddNeighbour(node10);
    }

    AddNode(node2);

    if (node3->IsActive())
    {
        node3->AddNeighbour(node0);
        node3->AddNeighbour(node5);
        node3->AddNeighbour(node6);

        AddNode(node3);
    }

    if (node4->IsActive())
    {
        node4->AddNeighbour(node1);
        node4->AddNeighbour(node3);
        node4->AddNeighbour(node7);
        node4->AddNeighbour(node5);

        AddNode(node4);
    }

    if (node5->IsActive())
    {
        node5->AddNeighbour(node2);
        node5->AddNeighbour(node3);
        node5->AddNeighbour(node8);

        AddNode(node5);
    }

    if (node3->IsActive())
    {
        node6->AddNeighbour(node3);
    }
    else
    {
        node6->AddNeighbour(node0);
    }
    if (node7->IsActive())
    {
        node6->AddNeighbour(node7);
    }
    else
    {
        node6->AddNeighbour(node8);
    }
    node6->AddNeighbour(node19);

    AddNode(node6);

    if (node7->IsActive())
    {
        node7->AddNeighbour(node4);
        node7->AddNeighbour(node6);
        node7->AddNeighbour(node8);

        AddNode(node7);
    }

    if (node5->IsActive())
    {
        node8->AddNeighbour(node5);
    }
    else
    {
        node8->AddNeighbour(node2);
    }
    if (node7->IsActive())
    {
        node8->AddNeighbour(node7);
    }
    else
    {
        node8->AddNeighbour(node6);
    }
    if (node13->IsActive())
    {
        node8->AddNeighbour(node13);
    }
    else
    {
        node8->AddNeighbour(node14);
    }

    AddNode(node8);

    if (node9->IsActive())
    {
        node9->AddNeighbour(node2);
        node9->AddNeighbour(node10);
        node9->AddNeighbour(node16);
        node9->AddNeighbour(node11);

        AddNode(node9);
    }

    if (node9->IsActive())
    {
        node10->AddNeighbour(node9);
    }
    else
    {
        node10->AddNeighbour(node2);
    }
    if (node12->IsActive())
    {
        node10->AddNeighbour(node12);
    }
    else
    {
        node10->AddNeighbour(node14);
    }
    if (node18->IsActive())
    {
        node10->AddNeighbour(node18);
    }
    else
    {
        node10->AddNeighbour(node17);
    }

    AddNode(node10);

    if (node11->IsActive())
    {
        node11->AddNeighbour(node9);
        node11->AddNeighbour(node5);
        node11->AddNeighbour(node13);
        node11->AddNeighbour(node12);

        AddNode(node11);
    }

    if (node12->IsActive())
    {
        node12->AddNeighbour(node10);
        node12->AddNeighbour(node11);
        node12->AddNeighbour(node14);

        AddNode(node12);
    }

    if (node13->IsActive())
    {
        node13->AddNeighbour(node8);
        node13->AddNeighbour(node11);
        node13->AddNeighbour(node14);

        AddNode(node13);
    }

    if (node13->IsActive())
    {
        node14->AddNeighbour(node13);
    }
    else
    {
        node14->AddNeighbour(node8);
    }
    if (node12->IsActive())
    {
        node14->AddNeighbour(node12);
    }
    else
    {
        node14->AddNeighbour(node10);
    }
    node14->AddNeighbour(node19);

    AddNode(node14);

    if (node1->IsActive())
    {
        node15->AddNeighbour(node0);
        node15->AddNeighbour(node16);
        node15->AddNeighbour(node17);

        AddNode(node15);
    }

    if (node16->IsActive())
    {
        node16->AddNeighbour(node1);
        node16->AddNeighbour(node15);
        node16->AddNeighbour(node18);
        node16->AddNeighbour(node9);

        AddNode(node16);
    }

    if (node15->IsActive())
    {
        node17->AddNeighbour(node15);
    }
    else
    {
        node17->AddNeighbour(node0);
    }
    if (node18->IsActive())
    {
        node17->AddNeighbour(node18);
    }
    else
    {
        node17->AddNeighbour(node10);
    }
    node17->AddNeighbour(node19);

    AddNode(node17);

    if (vertexActivityMask.test(18))
    {
        node18->AddNeighbour(node10);
        node18->AddNeighbour(node16);
        node18->AddNeighbour(node17);

        AddNode(node18);
    }

    node19->AddNeighbour(node6);
    node19->AddNeighbour(node14);
    node19->AddNeighbour(node17);

    AddNode(node19);

    if (edgesTable == nullptr)
    {
        edgesTable = std::make_unique<std::unordered_map<std::pair<int, int>, std::shared_ptr<EdgePoint>, Graph::pairHasher>>();

        std::shared_ptr<EdgePoint> edge0 = std::make_shared<EdgePoint>(*node0, *node1, 0);
        edgesTable->insert(std::make_pair(std::make_pair(edge0->GetFirstParentIndex(), edge0->GetSecondParentIndex()), edge0));

        std::shared_ptr<EdgePoint> edge1 = std::make_shared<EdgePoint>(*node0, *node2, 1);
        edgesTable->insert(std::make_pair(std::make_pair(edge1->GetFirstParentIndex(), edge1->GetSecondParentIndex()), edge1));

        std::shared_ptr<EdgePoint> edge2 = std::make_shared<EdgePoint>(*node1, *node2, 2);
        edgesTable->insert(std::make_pair(std::make_pair(edge2->GetFirstParentIndex(), edge2->GetSecondParentIndex()), edge2));

        std::shared_ptr<EdgePoint> edge3 = std::make_shared<EdgePoint>(*node0, *node3, 3);
        edgesTable->insert(std::make_pair(std::make_pair(edge3->GetFirstParentIndex(), edge3->GetSecondParentIndex()), edge3));

        std::shared_ptr<EdgePoint> edge4 = std::make_shared<EdgePoint>(*node1, *node4, 4);
        edgesTable->insert(std::make_pair(std::make_pair(edge4->GetFirstParentIndex(), edge4->GetSecondParentIndex()), edge4));

        std::shared_ptr<EdgePoint> edge5 = std::make_shared<EdgePoint>(*node2, *node10, 5);
        edgesTable->insert(std::make_pair(std::make_pair(edge5->GetFirstParentIndex(), edge5->GetSecondParentIndex()), edge5));

        std::shared_ptr<EdgePoint> edge6 = std::make_shared<EdgePoint>(*node0, *node6, 6);
        edgesTable->insert(std::make_pair(std::make_pair(edge6->GetFirstParentIndex(), edge6->GetSecondParentIndex()), edge6));

        std::shared_ptr<EdgePoint> edge7 = std::make_shared<EdgePoint>(*node3, *node4, 7);
        edgesTable->insert(std::make_pair(std::make_pair(edge7->GetFirstParentIndex(), edge7->GetSecondParentIndex()), edge7));

        std::shared_ptr<EdgePoint> edge8 = std::make_shared<EdgePoint>(*node3, *node5, 8);
        edgesTable->insert(std::make_pair(std::make_pair(edge8->GetFirstParentIndex(), edge8->GetSecondParentIndex()), edge8));
        edgesTable->insert(std::make_pair(std::make_pair(1, 7), edge8));

        std::shared_ptr<EdgePoint> edge9 = std::make_shared<EdgePoint>(*node4, *node5, 9);
        edgesTable->insert(std::make_pair(std::make_pair(edge9->GetFirstParentIndex(), edge9->GetSecondParentIndex()), edge9));

        std::shared_ptr<EdgePoint> edge10 = std::make_shared<EdgePoint>(*node2, *node8, 10);
        edgesTable->insert(std::make_pair(std::make_pair(edge10->GetFirstParentIndex(), edge10->GetSecondParentIndex()), edge10));

        std::shared_ptr<EdgePoint> edge11 = std::make_shared<EdgePoint>(*node3, *node6, 11);
        edgesTable->insert(std::make_pair(std::make_pair(edge11->GetFirstParentIndex(), edge11->GetSecondParentIndex()), edge11));

        std::shared_ptr<EdgePoint> edge12 = std::make_shared<EdgePoint>(*node4, *node7, 12);
        edgesTable->insert(std::make_pair(std::make_pair(edge12->GetFirstParentIndex(), edge12->GetSecondParentIndex()), edge12));

        std::shared_ptr<EdgePoint> edge13 = std::make_shared<EdgePoint>(*node5, *node8, 13);
        edgesTable->insert(std::make_pair(std::make_pair(edge13->GetFirstParentIndex(), edge13->GetSecondParentIndex()), edge13));

        std::shared_ptr<EdgePoint> edge14 = std::make_shared<EdgePoint>(*node6, *node7, 14);
        edgesTable->insert(std::make_pair(std::make_pair(edge14->GetFirstParentIndex(), edge14->GetSecondParentIndex()), edge14));

        std::shared_ptr<EdgePoint> edge15 = std::make_shared<EdgePoint>(*node6, *node8, 15);
        edgesTable->insert(std::make_pair(std::make_pair(edge15->GetFirstParentIndex(), edge15->GetSecondParentIndex()), edge15));

        std::shared_ptr<EdgePoint> edge16 = std::make_shared<EdgePoint>(*node7, *node8, 16);
        edgesTable->insert(std::make_pair(std::make_pair(edge16->GetFirstParentIndex(), edge16->GetSecondParentIndex()), edge16));

        std::shared_ptr<EdgePoint> edge17 = std::make_shared<EdgePoint>(*node2, *node9, 17);
        edgesTable->insert(std::make_pair(std::make_pair(edge17->GetFirstParentIndex(), edge17->GetSecondParentIndex()), edge17));

        std::shared_ptr<EdgePoint> edge18 = std::make_shared<EdgePoint>(*node2, *node10, 18);
        edgesTable->insert(std::make_pair(std::make_pair(edge18->GetFirstParentIndex(), edge18->GetSecondParentIndex()), edge18));

        std::shared_ptr<EdgePoint> edge19 = std::make_shared<EdgePoint>(*node9, *node10, 19);
        edgesTable->insert(std::make_pair(std::make_pair(edge19->GetFirstParentIndex(), edge19->GetSecondParentIndex()), edge19));

        std::shared_ptr<EdgePoint> edge20 = std::make_shared<EdgePoint>(*node9, *node11, 20);
        edgesTable->insert(std::make_pair(std::make_pair(edge20->GetFirstParentIndex(), edge20->GetSecondParentIndex()), edge20));

        std::shared_ptr<EdgePoint> edge21 = std::make_shared<EdgePoint>(*node10, *node12, 21);
        edgesTable->insert(std::make_pair(std::make_pair(edge21->GetFirstParentIndex(), edge21->GetSecondParentIndex()), edge21));

        std::shared_ptr<EdgePoint> edge22 = std::make_shared<EdgePoint>(*node5, *node11, 22);
        edgesTable->insert(std::make_pair(std::make_pair(edge22->GetFirstParentIndex(), edge22->GetSecondParentIndex()), edge22));

        std::shared_ptr<EdgePoint> edge23 = std::make_shared<EdgePoint>(*node5, *node12, 23);
        edgesTable->insert(std::make_pair(std::make_pair(edge23->GetFirstParentIndex(), edge23->GetSecondParentIndex()), edge23));
        edgesTable->insert(std::make_pair(std::make_pair(9, 13), edge23));

        std::shared_ptr<EdgePoint> edge24 = std::make_shared<EdgePoint>(*node11, *node12, 24);
        edgesTable->insert(std::make_pair(std::make_pair(edge24->GetFirstParentIndex(), edge24->GetSecondParentIndex()), edge24));

        std::shared_ptr<EdgePoint> edge25 = std::make_shared<EdgePoint>(*node11, *node13, 25);
        edgesTable->insert(std::make_pair(std::make_pair(edge25->GetFirstParentIndex(), edge25->GetSecondParentIndex()), edge25));

        std::shared_ptr<EdgePoint> edge26 = std::make_shared<EdgePoint>(*node12, *node14, 26);
        edgesTable->insert(std::make_pair(std::make_pair(edge26->GetFirstParentIndex(), edge26->GetSecondParentIndex()), edge26));

        std::shared_ptr<EdgePoint> edge27 = std::make_shared<EdgePoint>(*node8, *node13, 27);
        edgesTable->insert(std::make_pair(std::make_pair(edge27->GetFirstParentIndex(), edge27->GetSecondParentIndex()), edge27));

        std::shared_ptr<EdgePoint> edge28 = std::make_shared<EdgePoint>(*node8, *node14, 28);
        edgesTable->insert(std::make_pair(std::make_pair(edge28->GetFirstParentIndex(), edge28->GetSecondParentIndex()), edge28));

        std::shared_ptr<EdgePoint> edge29 = std::make_shared<EdgePoint>(*node13, *node14, 29);
        edgesTable->insert(std::make_pair(std::make_pair(edge29->GetFirstParentIndex(), edge29->GetSecondParentIndex()), edge29));

        std::shared_ptr<EdgePoint> edge30 = std::make_shared<EdgePoint>(*node0, *node15, 30);
        edgesTable->insert(std::make_pair(std::make_pair(edge30->GetFirstParentIndex(), edge30->GetSecondParentIndex()), edge30));

        std::shared_ptr<EdgePoint> edge31 = std::make_shared<EdgePoint>(*node0, *node17, 31);
        edgesTable->insert(std::make_pair(std::make_pair(edge31->GetFirstParentIndex(), edge31->GetSecondParentIndex()), edge31));

        std::shared_ptr<EdgePoint> edge32 = std::make_shared<EdgePoint>(*node15, *node17, 32);
        edgesTable->insert(std::make_pair(std::make_pair(edge32->GetFirstParentIndex(), edge32->GetSecondParentIndex()), edge32));

        std::shared_ptr<EdgePoint> edge33 = std::make_shared<EdgePoint>(*node15, *node16, 33);
        edgesTable->insert(std::make_pair(std::make_pair(edge33->GetFirstParentIndex(), edge33->GetSecondParentIndex()), edge33));
        
        std::shared_ptr<EdgePoint> edge34 = std::make_shared<EdgePoint>(*node1, *node16, 34);
        edgesTable->insert(std::make_pair(std::make_pair(edge34->GetFirstParentIndex(), edge34->GetSecondParentIndex()), edge34));

        std::shared_ptr<EdgePoint> edge35 = std::make_shared<EdgePoint>(*node1, *node18, 35);
        edgesTable->insert(std::make_pair(std::make_pair(edge35->GetFirstParentIndex(), edge35->GetSecondParentIndex()), edge35));
        edgesTable->insert(std::make_pair(std::make_pair(9, 15), edge35));

        std::shared_ptr<EdgePoint> edge36 = std::make_shared<EdgePoint>(*node16, *node18, 36);
        edgesTable->insert(std::make_pair(std::make_pair(edge36->GetFirstParentIndex(), edge36->GetSecondParentIndex()), edge36));

        std::shared_ptr<EdgePoint> edge37 = std::make_shared<EdgePoint>(*node9, *node16, 37);
        edgesTable->insert(std::make_pair(std::make_pair(edge37->GetFirstParentIndex(), edge37->GetSecondParentIndex()), edge37));

        std::shared_ptr<EdgePoint> edge38 = std::make_shared<EdgePoint>(*node17, *node18, 38);
        edgesTable->insert(std::make_pair(std::make_pair(edge38->GetFirstParentIndex(), edge38->GetSecondParentIndex()), edge38));

        std::shared_ptr<EdgePoint> edge39 = std::make_shared<EdgePoint>(*node10, *node17, 39);
        edgesTable->insert(std::make_pair(std::make_pair(edge39->GetFirstParentIndex(), edge39->GetSecondParentIndex()), edge39));

        std::shared_ptr<EdgePoint> edge40 = std::make_shared<EdgePoint>(*node10, *node18, 40);
        edgesTable->insert(std::make_pair(std::make_pair(edge40->GetFirstParentIndex(), edge40->GetSecondParentIndex()), edge40));

        std::shared_ptr<EdgePoint> edge41 = std::make_shared<EdgePoint>(*node17, *node19, 41);
        edgesTable->insert(std::make_pair(std::make_pair(edge41->GetFirstParentIndex(), edge41->GetSecondParentIndex()), edge41));

        std::shared_ptr<EdgePoint> edge42 = std::make_shared<EdgePoint>(*node6, *node19, 42);
        edgesTable->insert(std::make_pair(std::make_pair(edge42->GetFirstParentIndex(), edge42->GetSecondParentIndex()), edge42));

        std::shared_ptr<EdgePoint> edge43 = std::make_shared<EdgePoint>(*node14, *node19, 43);
        edgesTable->insert(std::make_pair(std::make_pair(edge43->GetFirstParentIndex(), edge43->GetSecondParentIndex()), edge43));
    }

    if (edgeToFaceTable == nullptr)
    {
        edgeToFaceTable = std::make_unique<std::unordered_map<int, std::unordered_set<int>>>();

        edgeToFaceTable->insert(std::make_pair(0, std::unordered_set<int>{0, 2}));
        edgeToFaceTable->insert(std::make_pair(1, std::unordered_set<int>{0, 2}));
        edgeToFaceTable->insert(std::make_pair(2, std::unordered_set<int>{0, 2}));
        edgeToFaceTable->insert(std::make_pair(3, std::unordered_set<int>{0, 3}));
        edgeToFaceTable->insert(std::make_pair(4, std::unordered_set<int>{0}));
        edgeToFaceTable->insert(std::make_pair(5, std::unordered_set<int>{0, 1}));
        edgeToFaceTable->insert(std::make_pair(5, std::unordered_set<int>{0, 1}));
        edgeToFaceTable->insert(std::make_pair(6, std::unordered_set<int>{0, 3}));
        edgeToFaceTable->insert(std::make_pair(7, std::unordered_set<int>{0}));
        edgeToFaceTable->insert(std::make_pair(8, std::unordered_set<int>{0}));
        edgeToFaceTable->insert(std::make_pair(9, std::unordered_set<int>{0}));
        edgeToFaceTable->insert(std::make_pair(10, std::unordered_set<int>{0, 1}));
        edgeToFaceTable->insert(std::make_pair(11, std::unordered_set<int>{0, 3}));
        edgeToFaceTable->insert(std::make_pair(12, std::unordered_set<int>{0}));
        edgeToFaceTable->insert(std::make_pair(13, std::unordered_set<int>{0, 1}));
        edgeToFaceTable->insert(std::make_pair(14, std::unordered_set<int>{0, 5}));
        edgeToFaceTable->insert(std::make_pair(15, std::unordered_set<int>{0, 5}));
        edgeToFaceTable->insert(std::make_pair(16, std::unordered_set<int>{0, 5}));
        edgeToFaceTable->insert(std::make_pair(17, std::unordered_set<int>{1, 2}));
        edgeToFaceTable->insert(std::make_pair(18, std::unordered_set<int>{1, 2}));
        edgeToFaceTable->insert(std::make_pair(19, std::unordered_set<int>{1, 2}));
        edgeToFaceTable->insert(std::make_pair(20, std::unordered_set<int>{1}));
        edgeToFaceTable->insert(std::make_pair(21, std::unordered_set<int>{1, 4}));
        edgeToFaceTable->insert(std::make_pair(22, std::unordered_set<int>{1}));
        edgeToFaceTable->insert(std::make_pair(23, std::unordered_set<int>{1}));
        edgeToFaceTable->insert(std::make_pair(24, std::unordered_set<int>{1}));
        edgeToFaceTable->insert(std::make_pair(25, std::unordered_set<int>{1}));
        edgeToFaceTable->insert(std::make_pair(26, std::unordered_set<int>{1, 4}));
        edgeToFaceTable->insert(std::make_pair(27, std::unordered_set<int>{1, 5}));
        edgeToFaceTable->insert(std::make_pair(28, std::unordered_set<int>{1, 5}));
        edgeToFaceTable->insert(std::make_pair(29, std::unordered_set<int>{1, 5}));
        edgeToFaceTable->insert(std::make_pair(30, std::unordered_set<int>{2, 3}));
        edgeToFaceTable->insert(std::make_pair(31, std::unordered_set<int>{2, 3}));
        edgeToFaceTable->insert(std::make_pair(32, std::unordered_set<int>{2, 3}));
        edgeToFaceTable->insert(std::make_pair(33, std::unordered_set<int>{2}));
        edgeToFaceTable->insert(std::make_pair(34, std::unordered_set<int>{2}));
        edgeToFaceTable->insert(std::make_pair(35, std::unordered_set<int>{2}));
        edgeToFaceTable->insert(std::make_pair(36, std::unordered_set<int>{2}));
        edgeToFaceTable->insert(std::make_pair(37, std::unordered_set<int>{2}));
        edgeToFaceTable->insert(std::make_pair(38, std::unordered_set<int>{2, 4}));
        edgeToFaceTable->insert(std::make_pair(39, std::unordered_set<int>{2, 4}));
        edgeToFaceTable->insert(std::make_pair(40, std::unordered_set<int>{2, 4}));
        edgeToFaceTable->insert(std::make_pair(40, std::unordered_set<int>{2, 4}));
        edgeToFaceTable->insert(std::make_pair(41, std::unordered_set<int>{3, 4}));
        edgeToFaceTable->insert(std::make_pair(42, std::unordered_set<int>{3, 5}));
        edgeToFaceTable->insert(std::make_pair(43, std::unordered_set<int>{4, 5}));
    }
}

std::shared_ptr<VertexPoint> Graph::GetEntry()
{
    return nodes->at(0);
}

void Graph::AddNode(std::shared_ptr<VertexPoint> node)
{
    nodes->at(node->GetIndex()) = node;
}

std::shared_ptr<std::array<std::shared_ptr<VertexPoint>, 20>> Graph::GetNodes()
{
    return nodes;
}

std::shared_ptr<EdgePoint> Graph::GetEdgePoint(int firstVertexIndex, int secondVertexIndex)
{
    int first = std::min(firstVertexIndex, secondVertexIndex);
	int second = std::max(firstVertexIndex, secondVertexIndex);
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

