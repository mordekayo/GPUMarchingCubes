#include "Graph13.hpp"
#include "../Points/VertexPoints/VertexPoint.hpp"
#include "../Math/Vector3.hpp"

Graph13::Graph13()
{

}

void Graph13::Create(const VertexActivityMask& vertexActivityMask)
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
    std::shared_ptr<VertexPoint> node9 = std::make_shared<VertexPoint>(Vector3(0.0f, 1.0f, 0.0f), 9, vertexActivityMask.test(9));
    std::shared_ptr<VertexPoint> node10 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 0.0f), 10, vertexActivityMask.test(10));
    std::shared_ptr<VertexPoint> node11 = std::make_shared<VertexPoint>(Vector3(0.0f, 1.0f, 1.0f), 11, vertexActivityMask.test(11));
    std::shared_ptr<VertexPoint> node12 = std::make_shared<VertexPoint>(Vector3(1.0f, 1.0f, 1.0f), 12, vertexActivityMask.test(12));
    
    nodes = std::make_shared<std::vector<std::shared_ptr<VertexPoint>>>(13);

    node0->AddNeighbour(node1);
    node0->AddNeighbour(node3);
    node0->AddNeighbour(node9);
    
    AddNode(node0);

    node1->AddNeighbour(node0);
    node1->AddNeighbour(node2);
    node1->AddNeighbour(node4);

    AddNode(node1);

    node2->AddNeighbour(node1);
    node2->AddNeighbour(node5);
    node2->AddNeighbour(node10);

    AddNode(node2);

    node3->AddNeighbour(node0);
    node3->AddNeighbour(node4);
    node3->AddNeighbour(node6);

    AddNode(node3);

    node4->AddNeighbour(node1);
    node4->AddNeighbour(node3);
    node4->AddNeighbour(node7);
    node4->AddNeighbour(node5);

    AddNode(node4);

    node5->AddNeighbour(node2);
    node5->AddNeighbour(node4);
    node5->AddNeighbour(node8);

    AddNode(node5);

    node6->AddNeighbour(node3);
    node6->AddNeighbour(node7);
    node6->AddNeighbour(node11);

    AddNode(node6);
    
    node7->AddNeighbour(node4);
    node7->AddNeighbour(node6);
    node7->AddNeighbour(node8);

    AddNode(node7);

    node8->AddNeighbour(node5);
    node8->AddNeighbour(node7);
    node8->AddNeighbour(node12);
    
    AddNode(node8);

    node9->AddNeighbour(node0);
    node9->AddNeighbour(node10);
    node9->AddNeighbour(node11);

    AddNode(node9);

    node10->AddNeighbour(node2);
    node10->AddNeighbour(node9);
    node10->AddNeighbour(node12);

    AddNode(node10);

    node11->AddNeighbour(node6);
    node11->AddNeighbour(node9);
    node11->AddNeighbour(node12);
    node11->AddNeighbour(node12);

    AddNode(node11);

    node12->AddNeighbour(node8);
    node12->AddNeighbour(node10);
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
    }
    if (edgeToFaceTable == nullptr)
    {
        edgeToFaceTable = std::make_unique<std::unordered_map<int, std::unordered_set<int>>>();

        edgeToFaceTable->insert(std::make_pair(0, std::unordered_set<int>{0, 2}));
        edgeToFaceTable->insert(std::make_pair(1, std::unordered_set<int>{0, 2}));
        edgeToFaceTable->insert(std::make_pair(2, std::unordered_set<int>{0, 1}));
        edgeToFaceTable->insert(std::make_pair(3, std::unordered_set<int>{0}));
        edgeToFaceTable->insert(std::make_pair(4, std::unordered_set<int>{0, 4}));
        edgeToFaceTable->insert(std::make_pair(5, std::unordered_set<int>{0}));
        edgeToFaceTable->insert(std::make_pair(6, std::unordered_set<int>{0}));
        edgeToFaceTable->insert(std::make_pair(7, std::unordered_set<int>{0, 1}));
        edgeToFaceTable->insert(std::make_pair(8, std::unordered_set<int>{0}));
        edgeToFaceTable->insert(std::make_pair(9, std::unordered_set<int>{0, 4}));
        edgeToFaceTable->insert(std::make_pair(10, std::unordered_set<int>{0, 5}));
        edgeToFaceTable->insert(std::make_pair(11, std::unordered_set<int>{0, 5}));
        edgeToFaceTable->insert(std::make_pair(12, std::unordered_set<int>{0, 1}));
        edgeToFaceTable->insert(std::make_pair(13, std::unordered_set<int>{0, 4}));
        edgeToFaceTable->insert(std::make_pair(14, std::unordered_set<int>{1, 5}));
        edgeToFaceTable->insert(std::make_pair(15, std::unordered_set<int>{4, 5}));
        edgeToFaceTable->insert(std::make_pair(16, std::unordered_set<int>{2, 3}));
        edgeToFaceTable->insert(std::make_pair(17, std::unordered_set<int>{1, 3}));      
        edgeToFaceTable->insert(std::make_pair(18, std::unordered_set<int>{3, 5}));
        edgeToFaceTable->insert(std::make_pair(19, std::unordered_set<int>{3, 4}));
    }
}
