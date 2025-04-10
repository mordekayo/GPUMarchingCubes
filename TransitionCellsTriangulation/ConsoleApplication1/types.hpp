#pragma once
#include <bitset>
#include <array>
#include <memory>
#include <cstdint>

constexpr int tableRowLength = 44;
constexpr int vertexActivityMaskLength = 20;
constexpr int tableSize = 1 << 20;
using VertexActivityMask = std::bitset<vertexActivityMaskLength>;
using TableRow = std::array<std::int8_t, tableRowLength>;
using TableArray = std::array<TableRow, tableSize>;