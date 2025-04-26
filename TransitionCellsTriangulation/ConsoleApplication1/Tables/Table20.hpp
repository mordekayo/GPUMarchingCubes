#pragma once

#include "Table.hpp"

class Table20 : public Table
{
	virtual std::unique_ptr<Graph> CreateGraph();
};