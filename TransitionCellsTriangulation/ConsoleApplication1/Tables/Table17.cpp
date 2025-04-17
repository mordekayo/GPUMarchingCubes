#include "Table17.hpp"
#include "../Graph/Graph17.hpp"

std::unique_ptr<Graph> Table17::CreateGraph()
{
    return std::make_unique<Graph17>();
}
