#include <iostream>
#include "Tables/Table13.hpp"
#include "types.hpp"

int main()
{
    Table13 table13;
    std::vector<int> mask({ 1,2,8,10,12 });
    TableRow row = table13.MakeRow(mask);
    for (auto index : row)
    {
        std::cout << (int)index << " ";
    }
    return 0;
}
