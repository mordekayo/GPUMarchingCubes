#include "Table13.hpp"
#include "../Graph/Graph13.hpp"

std::unique_ptr<Graph> Table13::CreateGraph()
{
    return std::make_unique<Graph13>();
}
