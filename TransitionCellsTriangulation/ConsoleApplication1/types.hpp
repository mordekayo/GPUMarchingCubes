#pragma once
#include <bitset>
#include <array>
#include <memory>
#include <cstdint>

constexpr int tableRowLength = 66;
constexpr int vertexActivityMaskLength = 13;
constexpr int tableSize = 1 << 13;
using VertexActivityMask = std::bitset<vertexActivityMaskLength>;
using TableRow = std::array<std::int8_t, tableRowLength>;
using TableArray = std::array<TableRow, tableSize>;

