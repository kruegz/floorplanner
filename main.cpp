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
#include "slicing_tree.h"

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

std::vector<Block> readBlocksFromFile(std::string filename)
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

void outputSolution(SlicingTree *st, std::string filename)
{
    // Get the final area of the minimum slicing tree
    double final_area = st->score();

    std::cout << "Final stree: " << st->toString() << std::endl;
    std::cout << "Final area: " << final_area << std::endl;


    // Compute white area
    double white_area = 0;

    for (Block *b : st->blocks)
    {
        // Only add up areas for original blocks
        if (b->blockName != "|" and b->blockName != "-")
        {
            double block_area = b->widths_heights[0].area();
            //std::cout << block_area << std::endl;
            white_area += block_area;
        }
    }

    //std::cout << "White area: " << white_area << std::endl;

    // Compute black area
    double black_area = final_area - white_area;

    std::cout << "Black area: " << black_area << std::endl;
    std::cout << "Black area percent: " << black_area / final_area << std::endl;

    // Write to output file

    std::ofstream out_file(filename);

    out_file << "Final area = " << final_area << std::endl;
    out_file << "Black area = " << black_area << std::endl << std::endl;

    out_file << "block_name lower_left(x,y)coordinate upper_right(x,y)coordinate" << std::endl;
    out_file << st->getCoords() << std::endl;

    out_file.close();

}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: floorplan <blocks_file>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string filename = argv[1];
   
    // Read blocks
    std::vector<Block> blocks = readBlocksFromFile(filename);

    // Create the output file
    std::string output_file = filename.substr(0,filename.find_last_of('.'))+".out";

    // Perform the floorplanning
    SlicingTree *st = floorplan(blocks);

    // Output the solution
    outputSolution(st, output_file);
    
    return 0;
}
