#include <array>
#include <string>
#include <memory>
#include <cmath>
#include <vector>
#include <bitset>

#include "Graph.hpp"

class Table
{
public:

	Table();
	Table(const Table& other) = delete;
	Table& operator=(const Table& other) = delete;

	void CreateEmpty();
	void Serialize(const std::string& fileName);
	void Deserialize(const std::string& fileName);

	float GetFilledRatio();
	void PrintFilledRatio();

	void Fill();

	TableRow MakeRow(const std::vector<int> activeVertexes);
	TableRow MakeRow(const VertexActivityMask& vertexActivityMask);

private:

	int filledCount = 0;

	std::unique_ptr<TableArray> table = nullptr;
};
