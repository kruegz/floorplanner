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

    // Create a slicing tree object
    SlicingTree stree(blocks);

    // Perform simulated annealing
    int temp = STARTING_TEMP * blocks.size();
    int freeze_temp = FREEZING_TEMP;
    int steps = NUM_MOVES_PER_STEP * blocks.size();
    int cost;

    double min_score = std::numeric_limits<double>::max();
    
    // Get the initial score
    double current_score = stree.score();
    

    SlicingTree min_stree;
    SlicingTree new_stree;


    while (temp > freeze_temp)
    {

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



void verticalNodeSizing(Block *a, Block *b, Block *result)
{
    // Sort input sets by height
    sortByWidth(a);
    sortByWidth(b);

    
    // Clear result sizes
    result->widths_heights.clear();


    int len_a = a->widths_heights.size();
    int len_b = b->widths_heights.size();

    int i = 0, j = 0;
    double new_width, new_height;


    WidthHeight a_wh, b_wh;

    // Traverse possible width and height combinations
    while (i < len_a and j < len_b)
    {

        a_wh = a->widths_heights[i];
        b_wh = b->widths_heights[j];

        // Perform vertical slice sizing
        new_width = a_wh.width + b_wh.width;
        new_height = std::max(a_wh.height, b_wh.height);

        // Create new width height object storing which indexes 
        WidthHeight new_wh(new_width, new_height, i, j);

        // Add to result block
        result->widths_heights.push_back(new_wh);

        // Increment correct index
        if (new_height == a_wh.height)
        {
            i++;
        }
        if (new_height == b_wh.height)
        {
            j++;
        }
    }
}

void horizontalNodeSizing(Block *a, Block *b, Block *result)
{
    // Sort input sets by height
    sortByHeight(a);
    sortByHeight(b);

    // Clear result sizes
    result->widths_heights.clear();

    int len_a = a->widths_heights.size();
    int len_b = b->widths_heights.size();

    int i = 0, j = 0;
    double new_width, new_height;


    WidthHeight a_wh, b_wh;

    // Traverse set of widths and height
    while (i < len_a and j < len_b)
    {

        a_wh = a->widths_heights[i];
        b_wh = b->widths_heights[j];

        // Compute horizontal slice
        new_width = std::max(a_wh.width, b_wh.width);
        new_height = a_wh.height + b_wh.height;

        // Create width height object storing which indexes 
        WidthHeight new_wh(new_width, new_height, i, j);

        // Add to result block
        result->widths_heights.push_back(new_wh);

        // Increment correct index
        if (new_width == a_wh.width)
        {
            i++;
        }
        if (new_width == b_wh.width)
        {
            j++;
        }
    }
}

