#include <iostream>
//#include "Tables/Table13.hpp"
//#include "Tables/Table17.hpp"
#include "Tables/Table20.hpp"
#include "types.hpp"


int main()
{
    //Table17 table17;
    //std::vector<int> mask({6});
    //TableRow row = table17.MakeRow(mask);
    //for (auto index : row)
    //{
    //    std::cout << (int)index << " ";
    //}

    //Table13 table13;
    //std::vector<int> mask({3});
    //TableRow row = table13.MakeRow(mask);
    //for (auto index : row)
    //{
    //    std::cout << (int)index << " ";
    //}

    Table20 table20;
    //std::vector<int> mask({ 0, 1, 4, 9, 11, 12 });
    std::vector<int> mask({ 6, 13, 15 });
    TableRow row = table20.MakeRow(mask);
    for (auto index : row)
    {
        std::cout << (int)index << " ";
    }
    return 0;
}
