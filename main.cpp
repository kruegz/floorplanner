#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <cmath>

#include "block.h"
#include "floorplan.h"
#include "slicing_tree.h"

// Helper function to split a string
template<typename Out> 
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

// Helper function to split a string
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

// Parse a line into a hard bloc
Block parseLineHard(std::string line)
{
    std::vector<std::string> split_line = split(line, ' ');

    std::string name;
    int width, height;

    // Parse the line
    name = split_line[0];

    std::string width_str, height_str;

    // Get the width and height
    width_str = split_line[6];
    width_str.pop_back();
    height_str = split_line[7];
    height_str.pop_back();
    height_str.erase(height_str.begin());

    width = stof(width_str);
    height = stof(height_str);

    // Create a width height object
    WidthHeight wh(width, height, -1, -1);

    std::vector<WidthHeight> whs;

    // Add the width height and rotation to the set of widths and height
    whs.push_back(wh);
    whs.push_back(wh.rotate());
   
    // Create a new block with the set of possible widths and heights
    Block b(name, false, whs);

    return b;
}

// Parse a line into a soft block
Block parseLineSoft(std::string line)
{
    // Split the line by space
    std::vector<std::string> split_line = split(line, ' ');

    std::string name = split_line[0];

    // Get the area and aspect ratios from the line
    double area = stof(split_line[2]);
    double min_aspect = stof(split_line[3]);
    double max_aspect = stof(split_line[4]);

    // Set of possible block widths and heights
    std::vector<WidthHeight> whs;
    
    if (min_aspect == max_aspect)
    {
        // Aspect ratios are the same, only add one size

        double width = sqrt(area * min_aspect);
        double height = sqrt(area / min_aspect);

        whs.push_back(WidthHeight(width, height, -1, -1));
    }
    else if (max_aspect <= 1 or min_aspect >= 1)
    {
        // 1 is not in range of aspects, only add two aspects
        
        double width = sqrt(area * min_aspect);
        double height = sqrt(area / min_aspect);

        whs.push_back(WidthHeight(width, height, -1, -1));
        
        width = sqrt(area * max_aspect);
        height = sqrt(area / max_aspect);

        whs.push_back(WidthHeight(width, height, -1, -1));
    }
    else
    {
        // Add min_aspect, max_aspect, and aspect=1 size blocks
        double width = sqrt(area * min_aspect);
        double height = sqrt(area / min_aspect);

        whs.push_back(WidthHeight(width, height, -1, -1));
        
        width = sqrt(area * max_aspect);
        height = sqrt(area / max_aspect);

        whs.push_back(WidthHeight(width, height, -1, -1));

        width = sqrt(area);
        height = sqrt(area);

        whs.push_back(WidthHeight(width, height, -1, -1));
    }

    // Create a new block with the set of widths and heights
    Block b(name, false, whs);

    return b;
}

// Parse a line into either a soft or hard block
Block parseLine(std::string line)
{
    std::vector<std::string> split_line = split(line, ' ');

    // Determine if soft or hard block
    if (split_line[1] == "hardrectilinear")
    {
        return parseLineHard(line);
    }
    else
    {
        return parseLineSoft(line);
    }
}

// Read a file into a bunch of blocks
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

        // Read blocks
        for (int i = 0; i < num_hard_blocks + num_soft_blocks; i++)
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

// Output a solution in the form of a slicing tree into an output file
void outputSolution(SlicingTree *st, std::string filename)
{
    // Get the final area of the minimum slicing tree
    double final_area = st->score();


    // Compute white area
    double white_area = 0;

    for (Block *b : st->blocks)
    {
        // Only add up areas for original blocks
        if (b->blockName != "|" and b->blockName != "-")
        {
            double block_area = b->widths_heights[0].area();
            white_area += block_area;
        }
    }


    // Compute black area
    double black_area = final_area - white_area;


    // Print the percentage
    std::cout << black_area / final_area << std::endl;

    // Write to output file

    std::ofstream out_file(filename);

    out_file << "Final area = " << final_area << std::endl;
    out_file << "Black area = " << black_area << std::endl << std::endl;

    out_file << "block_name lower_left(x,y)coordinate upper_right(x,y)coordinate" << std::endl;

    // Output all coordinates
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
