#include "Table20.hpp"
#include "../Graph/Graph20.hpp"

std::unique_ptr<Graph> Table20::CreateGraph()
{
	return std::make_unique<Graph20>();
}
