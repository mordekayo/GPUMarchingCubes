#include <iostream>
//#include "Tables/Table13.hpp"
#include "Tables/Table17.hpp"
//#include "Tables/Table20.hpp"
#include "types.hpp"

#include <fstream>

//void SerializeDefaultTriTable(const std::string& fileName)
//{
//    std::string filename{ fileName };
//    std::fstream fileStream{ filename, fileStream.binary | fileStream.trunc | fileStream.out };
//
//    if (!fileStream.is_open())
//    {
//        std::cout << "Failed to open " << filename << '\n';
//        return;
//    }
//
//    fileStream.write(reinterpret_cast<char*>(triTable), sizeof(std::int8_t) * 256 * 16);
//
//    fileStream.close();
//}

int main()
{
    Table17 table17;
    //std::vector<int> mask({0,2,3,4,5,6,7,10,11});
    //auto row = table13.MakeRow(mask);
    //int i = 0;
    //for (; row[i] != -1; ++i)
    //{
    //    std::cout << (int)row[i] << ", ";
    //}
    /*std::vector<int> mask({ 0,1,2,3,4,5,6,7,9,10 });
    auto row = table13.MakeRow(mask);
    int i = 0;
    for (; row[i] != -1; ++i)
    {
        std::cout << (int)row[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << i;*/
    table17.CreateEmpty();
    table17.Fill();
    std::cout << table17.GetMaxEdgePointsCount();
    table17.Serialize("Table15.bin");
    table17.SerializeAsText("Table15.txt");

    //Table20 table20;
    //table20.CreateEmpty();
    //table20.Fill();
    //std::cout << table20.GetMaxEdgePointsCount();
    //table20.Serialize("Table14.bin");
    //table20.SerializeAsText("Table14.txt");

    //SerializeDefaultTriTable("TriTable.bin");

    return 0;
}
