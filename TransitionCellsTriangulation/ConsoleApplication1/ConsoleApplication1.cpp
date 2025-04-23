#include <iostream>
//#include "Tables/Table13.hpp"
#include "Tables/Table17.hpp"
#include "types.hpp"


int main()
{
    Table17 table17;
    std::vector<int> mask({8, 16});
    TableRow row = table17.MakeRow(mask);
    for (auto index : row)
    {
        std::cout << (int)index << " ";
    }

    //Table13 table13;
    //std::vector<int> mask({0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12});
    //TableRow row = table13.MakeRow(mask);
    //for (auto index : row)
    //{
    //    std::cout << (int)index << " ";
    //}
    return 0;
}
