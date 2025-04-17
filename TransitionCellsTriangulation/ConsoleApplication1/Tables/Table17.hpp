#pragma once

#include "Table.hpp"
#include <memory>

class Table17 : public Table
{
protected:

	virtual std::unique_ptr<Graph> CreateGraph();
};