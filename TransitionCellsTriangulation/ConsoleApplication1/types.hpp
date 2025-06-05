#pragma once
#include <bitset>
#include <array>
#include <memory>
#include <cstdint>

constexpr int tableRowLength = 120;
constexpr int vertexActivityMaskLength = 15;
constexpr int tableSize = 1 << 15;
using VertexActivityMask = std::bitset<vertexActivityMaskLength>;
using TableRow = std::array<std::int8_t, tableRowLength>;
using TableArray = std::array<TableRow, tableSize>;

