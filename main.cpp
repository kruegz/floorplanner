#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "block.h"
#include "floorplan.h"

template<typename Out> 
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

Block parseLine(std::string line)
{
    std::vector<std::string> split_line = split(line, ' ');

    std::string name;
    int width, height;


    name = split_line[0];

    std::string width_str, height_str;

    width_str = split_line[6];
    width_str.pop_back();
    height_str = split_line[7];
    height_str.pop_back();
    height_str.erase(height_str.begin());

    width = stof(width_str);
    height = stof(height_str);

    WidthHeight wh(width, height, -1, -1);

    std::vector<WidthHeight> whs;

    whs.push_back(wh);
    whs.push_back(wh.rotate());
    
    Block b(name, false, whs);

    return b;
}

std::vector<Block> readBlocksFromFile(char *filename)
{
    std::vector<Block> blocks;

    // Open file
    std::ifstream fin(filename);


    if (fin.is_open())
    {
        std::string line, garb;

        // Ignore first 4 lines
        for (int i = 0; i < 5; i++)
        {
            getline(fin, line);
        }

        int num_soft_blocks, num_hard_blocks;

        // Read number of blocks
        fin >> garb >> garb >> num_soft_blocks;
        fin >> garb >> garb >> num_hard_blocks;

        getline(fin, line);
        getline(fin, line);

        std::string name;

        // Read soft blocks
        for (int i = 0; i < num_soft_blocks; i++)
        {
            continue;
        }
        
        // Read hard blocks
        for (int i = 0; i < num_hard_blocks; i++)
        {
            getline(fin, line);

            Block new_block = parseLine(line);

            blocks.push_back(new_block);
            
        }


    }
    else
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    fin.close();

    return blocks;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: floorplan <blocks_file>" << std::endl;
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    
    std::vector<Block> blocks = readBlocksFromFile(filename);

    //for (Block& b : blocks)
    //{
        //std::cout << b << std::endl;
    //}

    //std::cout << "==================================" << std::endl;
    
    //std::vector<Block>::const_iterator start = blocks.begin();
    //std::vector<Block>::const_iterator end = blocks.begin() + 4;
    //std::vector<Block> a(start, end);
    ////a.push_back(blocks[0]);

    //start = blocks.begin() + 5;
    //end = blocks.end();
    //std::vector<Block> b(start, end);
    //a.push_back(blocks[1]);

    //std::vector<Block> vert = verticalNodeSizing(a, b);

    //std::vector<std::vector <Block>> test_blocks;

    //std::vector<Block> b1;
    //b1.push_back(Block("b1", false, 3, 4));
    //test_blocks.push_back(b1);
    //std::vector<Block> b2;
    //b2.push_back(Block("b2", false, 2, 1));
    //test_blocks.push_back(b2);
    //std::vector<Block> b3;
    //b3.push_back(Block("b3", false, 5, 9));
    //test_blocks.push_back(b3);
    //std::vector<Block> b4;
    //b4.push_back(Block("b4", false, 2, 2));
    //test_blocks.push_back(b4);

    //std::vector<int> plan;
    //plan.push_back(0);
    //plan.push_back(1);
    //plan.push_back(HORIZONTAL_SLICE);
    //plan.push_back(2);
    //plan.push_back(VERTICAL_SLICE);
    //plan.push_back(3);
    //plan.push_back(HORIZONTAL_SLICE);

    //std::vector<Block> v = verticalNodeSizing(b1, b2);



    //for (Block& b : vert)
    //{
        //std::cout << b << std::endl;
    //}

    std::vector<Block> final_blocks = floorplan(blocks);

    

    return 0;
}
