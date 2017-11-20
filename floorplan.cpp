#include <iostream>
#include <vector>
#include <cassert>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <climits>
#include <fstream>
#include <algorithm>
#include <limits>

#include "floorplan.h"
#include "block.h"
#include "slicing_tree.h"


SlicingTree* floorplan(std::vector<Block> blocks)
{

    srand(time(NULL));
    std::vector<Block> final_blocks;
    
    // TEST ///////////////////////////////////////////////////////////////////

    //static const int arr[] = {5, 4, VERTICAL_SLICE, 7, VERTICAL_SLICE, 8, 0, HORIZONTAL_SLICE, VERTICAL_SLICE, 3, 6, VERTICAL_SLICE, 1, 2, VERTICAL_SLICE, HORIZONTAL_SLICE, VERTICAL_SLICE, 9, VERTICAL_SLICE};

    //Block *barr[] = {&blocks[5], &blocks[4], new Block("|"), &blocks[7], new Block("|"), &blocks[8], &blocks[0], new Block("-"), new Block("|"), &blocks[3], &blocks[6], new Block("|"), &blocks[1], &blocks[2], new Block("|"), new Block("-"), new Block("|"), &blocks[9], new Block("|")};
   
    //std::vector<Block*> testTree (barr, barr + sizeof(barr) / sizeof(barr[0]) );

    //SlicingTree test_stree(testTree);
    //std::cout << test_stree.toString() << std::endl;

    //double test_score = test_stree.score();

    ////std::cout << *test_stree.blocks[test_stree.blocks.size() - 1] << std::endl;

    ////double test_score = test_stree.blocks[test_stree.blocks.size() - 1]->widths_heights[min_index].width * test_stree.blocks[test_stree.blocks.size() - 1]->widths_heights[min_index].height;

    //std::cout << "Test score: " << test_score << std::endl;
    //assert(test_score == 238290);
    //std::cout << "Test passed!" << std::endl;

    // ENDTEST ////////////////////////////////////////////////////////////////

    

    // Create a slicing tree object
    SlicingTree stree(blocks);
    
    //std::cout << "Initial slicing tree:" << std::endl;
    //std::cout << stree.toString() << std::endl;

    // Perform simulated annealing
    int temp = STARTING_TEMP * blocks.size();
    int freeze_temp = FREEZING_TEMP;
    int steps = NUM_MOVES_PER_STEP * blocks.size();
    int cost;

    double min_score = std::numeric_limits<double>::max();
    
    // Get the initial score
    double current_score = stree.score();
    
    //std::cout << "Initial score: " << current_score << std::endl;

    SlicingTree min_stree;
    SlicingTree new_stree;

    while (temp > freeze_temp)
    {
        //std::cout << temp << std::endl;

        for (int i = 0; i < steps; i++)
        {
            // Copy the current tree
            new_stree = stree;

            // Make a move on the new slicing tree and get the new score
            double new_score = new_stree.makeMove();

            // New tree should be a valid slicing tree
            assert(new_stree.isValid());

            // Check if is a new minimum
            if (new_score < min_score)
            {
                min_score = new_score;
                min_stree = new_stree;
            }

            // Compute cost
            cost = new_score - current_score;

            // Check if move is acceptable with current cost
            if (acceptMove(cost, temp))
            {
                // Accept the move

                // Save the slicing tree
                stree = new_stree;
                
                // Update current score
                current_score = new_score;
            }
        }

        // Cool down
        temp = COOLING_FACTOR * temp;
    }

    SlicingTree *result_stree = new SlicingTree;
    *result_stree = min_stree;


    return result_stree;
}


bool acceptMove(int cost, int temp)
{
    if (cost < 0)
    {
        return true;
    }
    else
    {
        double boltz = exp(-1 * cost / (boltz_k * temp));

        double r = (rand() / (RAND_MAX + 1.0));

        return (r < boltz);
    }
}


void sortByWidth(Block *b)
{
    std::sort(
        b->widths_heights.begin(), 
        b->widths_heights.end(), 
        [](const WidthHeight &l, const WidthHeight &r){ return l.width < r.width; }
    );
}

void sortByHeight(Block *b)
{
    std::sort(
        b->widths_heights.begin(), 
        b->widths_heights.end(), 
        [](const WidthHeight &l, const WidthHeight &r){ return l.height < r.height; }
    );
}

void printBlocks(std::vector<Block> blocks)
{
    for (Block& b : blocks)
    {
        std::cout << b << std::endl;
    }
}


void verticalNodeSizing(Block *a, Block *b, Block *result)
{
    // Sort input sets by height
    sortByWidth(a);
    sortByWidth(b);

    //std::cout << "=================" << std::endl;
    //printBlocks(a);
    //std::cout << "=================" << std::endl;
    //printBlocks(b);
    //std::cout << "=================" << std::endl;
    
    // Clear result sizes
    result->widths_heights.clear();


    int len_a = a->widths_heights.size();
    int len_b = b->widths_heights.size();

    //std::cout << len_a << " " << len_b << std::endl;

    int i = 0, j = 0;
    double new_width, new_height;


    WidthHeight a_wh, b_wh;

    while (i < len_a and j < len_b)
    {
        //Block *current_a = a[i];
        //Block *current_b = b[j];

        a_wh = a->widths_heights[i];
        b_wh = b->widths_heights[j];

        //std::cout << "a " << current_a << std::endl;
        //std::cout << "b " << current_b << std::endl;

        new_width = a_wh.width + b_wh.width;
        new_height = std::max(a_wh.height, b_wh.height);

        //Block *new_block = new Block(current_a->blockName + "-" + current_b->blockName, false, new_height, new_width, current_a, current_b, HORIZONTAL_SLICE);
        
        WidthHeight new_wh(new_width, new_height, i, j);

        result->widths_heights.push_back(new_wh);
        //result->areas.push_back(new_width * new_height);

        //std::cout << "new " << new_block << std::endl;

        //final_blocks.push_back(new_block);

        if (new_height == a_wh.height)
        {
            i++;
        }
        if (new_height == b_wh.height)
        {
            j++;
        }
    }


    //std::cout << "END HORIZONTAL SLICE" << std::endl;
    //return final_blocks;
}

void horizontalNodeSizing(Block *a, Block *b, Block *result)
{
    //std::cout << "START HORIZONTAL SLICE" << std::endl;
    // Sort input sets by height
    sortByHeight(a);
    sortByHeight(b);

    //std::cout << "=================" << std::endl;
    //printBlocks(a);
    //std::cout << "=================" << std::endl;
    //printBlocks(b);
    //std::cout << "=================" << std::endl;

    // Clear result sizes
    result->widths_heights.clear();


    int len_a = a->widths_heights.size();
    int len_b = b->widths_heights.size();

    //std::cout << len_a << " " << len_b << std::endl;

    int i = 0, j = 0;
    double new_width, new_height;


    WidthHeight a_wh, b_wh;

    while (i < len_a and j < len_b)
    {
        //std::cout << i << " " << j << std::endl;
        //Block *current_a = a[i];
        //Block *current_b = b[j];

        a_wh = a->widths_heights[i];
        b_wh = b->widths_heights[j];

        //std::cout << "a " << current_a << std::endl;
        //std::cout << "b " << current_b << std::endl;

        new_width = std::max(a_wh.width, b_wh.width);
        new_height = a_wh.height + b_wh.height;

        //Block *new_block = new Block(current_a->blockName + "-" + current_b->blockName, false, new_height, new_width, current_a, current_b, HORIZONTAL_SLICE);
        
        WidthHeight new_wh(new_width, new_height, i, j);

        result->widths_heights.push_back(new_wh);
        //result->areas.push_back(new_width * new_height);

        //std::cout << "new " << new_block << std::endl;

        //final_blocks.push_back(new_block);

        //std::cout << new_width << " " << a_wh.width << " " << b_wh.width << std::endl;

        if (new_width == a_wh.width)
        {
            i++;
        }
        if (new_width == b_wh.width)
        {
            j++;
        }
    }


    //std::cout << "END HORIZONTAL SLICE" << std::endl;
    //return final_blocks;
}

