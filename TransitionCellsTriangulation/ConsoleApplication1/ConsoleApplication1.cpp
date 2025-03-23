#include "table.hpp"
#include <iostream>

int main()
{
    Table table;
    table.CreateEmpty();
    std::vector<int> mask({ 9,10,12 });
    TableRow row = table.MakeRow(mask);
    for (auto index : row)
    {
        std::cout << (int)index << " ";
    }
    return 0;
}
