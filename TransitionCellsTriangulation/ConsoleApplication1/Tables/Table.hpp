#include <array>
#include <string>
#include <memory>
#include <cmath>
#include <vector>
#include <bitset>
#include "../types.hpp"

class Graph;

class Table
{
public:

	Table();
	Table(const Table& other) = delete;
	Table& operator=(const Table& other) = delete;

	void CreateEmpty();
	void Serialize(const std::string& fileName);
	void SerializeAsText(const std::string& fileName);
	void Deserialize(const std::string& fileName);

	float GetFailedRatio();
	void PrintFailedRatio();

	void Fill();

	TableRow MakeRow(const std::vector<int> activeVertexes);
	TableRow MakeRow(const VertexActivityMask& vertexActivityMask, bool& sucess);

	int GetMaxEdgePointsCount();

protected:

	virtual std::unique_ptr<Graph> CreateGraph() = 0;

private:

	int failedCount = 0;

	std::unique_ptr<TableArray> table = nullptr;

	int maxEdgePointsCount = 0;
};
