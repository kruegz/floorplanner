#include <iostream>
#include <vector>
#include <cassert>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <climits>

#include "floorplan.h"
#include "block.h"
#include "slicing_tree.h"

std::vector<Block> floorplan(std::vector<Block> blocks)
{

    srand(time(NULL));
    std::vector<Block> final_blocks;

    // Separate blocks and add their rotations

    std::vector<std::vector <Block>> separated_blocks;

    for (Block b : blocks)
    {
        std::vector<Block> new_blocks;

        new_blocks.push_back(b);
        new_blocks.push_back(b.rotate());

        separated_blocks.push_back(new_blocks);
    }
    
    // TEST ///////////////////////////////////////////////////////////////////

    //static const int arr[] = {5, 4, VERTICAL_SLICE, 7, VERTICAL_SLICE, 8, 0, HORIZONTAL_SLICE, VERTICAL_SLICE, 3, 6, VERTICAL_SLICE, 1, 2, VERTICAL_SLICE, HORIZONTAL_SLICE, VERTICAL_SLICE, 9, VERTICAL_SLICE};
    //std::vector<int> testTree (arr, arr + sizeof(arr) / sizeof(arr[0]) );
    
    //printSlicingTree(testTree);
    //int test_score = scoreSlicingTree(testTree, separated_blocks);
    //std::cout << "Test score: " << test_score << std::endl;

    // ENDTEST ////////////////////////////////////////////////////////////////

    
    // Build an initial slicing tree
    std::vector<int> slicingTree;
    std::vector<int> newSlicingTree;

    slicingTree.push_back(0);

    for (int i = 1; i < blocks.size(); i++)
    {
        slicingTree.push_back(i);

        if (i % 2)
        {
            slicingTree.push_back(HORIZONTAL_SLICE);
        }
        else
        {
            slicingTree.push_back(VERTICAL_SLICE);
        }
    }

    // Create a slicing tree object
    SlicingTree stree(slicingTree, separated_blocks);
    
    std::cout << stree.toString() << std::endl;

    // Perform simulated annealing
    int temp = STARTING_TEMP;
    int freeze_temp = FREEZING_TEMP;
    int cost;

    int min_score = INT_MAX;
    SlicingTree min_stree;

    while (temp > freeze_temp)
    {
        //std::cout << "Temp: " << temp << std::endl;

        Block current_block = stree.score();

        int current_score = current_block.area();

        for (int i = 0; i < NUM_MOVES_PER_STEP; i++)
        {
            //for (int &i : slicingTree)
            //{
                //std::cout << i << " ";
            //}
            //std::cout << std::endl;

            SlicingTree new_stree = stree;

            new_stree.makeMove();

            //for (int &i : newSlicingTree)
            //{
                //std::cout << i << " ";
            //}
            //std::cout << std::endl;

            assert(new_stree.isValid());

            Block new_block = new_stree.score();
 
            int new_score = new_block.area();

            if (new_score < min_score)
            {
                min_score = new_score;
                min_stree = new_stree;
            }

            cost = new_score - current_score;
            //std::cout << "Cost: " << cost << std::endl;

            if (acceptMove(cost, temp))
            {
                // Accept the move
                stree = new_stree;
                current_score = new_score;
                //std::cout << "accepted" << std::endl;
            }
        }

        // Cool down
        temp = COOLING_FACTOR * temp;
    }



    Block final_block = stree.score();
    int final_score = final_block.area();

    std::cout << "Final area: " << final_score << std::endl;
    std::cout << stree.toString() << std::endl;

    std::cout << "Min area: " << min_score << std::endl;
    std::cout << min_stree.toString() << std::endl;

    return final_blocks;
}

//std::vector<int> makeMove(std::vector<int> slicingTree)
//{
    //std::vector<int> newSlicingTree = slicingTree;

    //bool validMove = false;

    //while (not validMove)
    //{
        //int r1 = rand() % newSlicingTree.size();
        //int r2 = rand() % newSlicingTree.size();

        //std::vector<int> testSlicingTree = newSlicingTree;

        //if (testSlicingTree[r1] < 0)
        //{
            //if (testSlicingTree[r1] == HORIZONTAL_SLICE)
            //{
                //testSlicingTree[r1] = VERTICAL_SLICE;
            //}
            //else
            //{
                //testSlicingTree[r1] = HORIZONTAL_SLICE;
            //}
            //validMove = true;
            //break;
        //}

        //int t = testSlicingTree[r1];
        //testSlicingTree[r1] = testSlicingTree[r2];
        //testSlicingTree[r2] = t;

        //if (isValidSlicingTree(testSlicingTree))
        //{
            //validMove = true;
            //newSlicingTree = testSlicingTree;
            //break;
        //}
        //else
        //{
            //continue;
        //}

        //// Perform a random move
        //int move_type = rand() % 3;

        ////std::cout << "Move type: " << move_type << std::endl;


        //if (move_type == 0)
        //{
            //// Move type 0:
            //// Exchange two operands that have no other operands in between

            //int rand_index = rand() % newSlicingTree.size();        
            
            //// Make sure index is not a slice
            //if (newSlicingTree[rand_index] >= 0)
            //{
                //// Choose a direction
                //int rand_dir = rand() % 2;

                //if (rand_dir == 1)
                //{
                    //// Move positively to find a non-slice
                    //for (int i = 1; (rand_index + i) < newSlicingTree.size(); i++)
                    //{
                        //if ((rand_index + i) < newSlicingTree.size() and newSlicingTree[rand_index + i] >= 0)
                        //{
                            //// Swap indexes
                            //int temp = newSlicingTree[rand_index + i];
                            //newSlicingTree[rand_index + i] = newSlicingTree[rand_index];
                            //newSlicingTree[rand_index] = temp;

                            //validMove = true;
                            //break;
                        //}
                    //}
                //}
                //else
                //{
                    //// Move negatively to find a non-slice
                    //for (int i = 1; (rand_index - i) >= 0; i++)
                    //{
                        //if ((rand_index - i) >= 0 and newSlicingTree[rand_index - i] >= 0)
                        //{
                            //// Swap indexes
                            //int temp = newSlicingTree[rand_index - i];
                            //newSlicingTree[rand_index - i] = newSlicingTree[rand_index];
                            //newSlicingTree[rand_index] = temp;

                            //validMove = true;
                            //break;
                        //}
                    //}
                //}
            //}
        //}
        //else if (move_type == 1)
        //{
            //// Move type 1:
            //// Complement a series of operators between two operands

            //assert(newSlicingTree.size() >= 5);

            //// Choose a random index with a buffer of two items on either side
            //int rand_index = rand() % (newSlicingTree.size() - 4) + 2;

            //// Item at index must not be a slice
            //if (newSlicingTree[rand_index] >= 0)
            //{
                //// Check if negative direction contains an operator
                //if (newSlicingTree[rand_index - 1] < 0)
                //{
                    //int ind = rand_index - 2;
                    //bool valid = false;

                    //// Traverse in negative direction to find first non-operator
                    //while (ind >= 0)
                    //{
                        //if (newSlicingTree[ind] >= 0)
                        //{
                            //valid = true;
                            //break;
                        //}
                        //ind--;
                    //}

                    //// Check if a valid sequence of operators was found
                    //if (valid)
                    //{
                        //// Invert all operators
                        
                        //for (int i = rand_index - 1; i > ind; i--)
                        //{
                            //assert(newSlicingTree[i] == HORIZONTAL_SLICE or newSlicingTree[i] == VERTICAL_SLICE);

                            //if (newSlicingTree[i] == HORIZONTAL_SLICE)
                            //{
                                //newSlicingTree[i] = VERTICAL_SLICE;
                            //}
                            //else
                            //{
                                //newSlicingTree[i] = HORIZONTAL_SLICE;
                            //}
                        //}

                        //validMove = true;
                    //}
                //}
                //// Check for operand in positive direction
                //else if (newSlicingTree[rand_index + 1] < 0) 
                //{
                    //int ind = rand_index + 2;
                    //bool valid = false;
                    
                    //// Find next non-opeator
                    //while (ind < newSlicingTree.size())
                    //{
                        //if (newSlicingTree[ind] >= 0)
                        //{
                            //valid = true;
                            //break;
                        //}
                        //ind++;
                    //}

                    //if (valid)
                    //{
                        //// Invert sequence of operators
                        //for (int i = rand_index + 1; i < ind; i++)
                        //{
                            //assert(newSlicingTree[i] == HORIZONTAL_SLICE or newSlicingTree[i] == VERTICAL_SLICE);

                            //if (newSlicingTree[i] == HORIZONTAL_SLICE)
                            //{
                                //newSlicingTree[i] = VERTICAL_SLICE;
                            //}
                            //else
                            //{
                                //newSlicingTree[i] = HORIZONTAL_SLICE;
                            //}
                        //}

                        //validMove = true;
                    //}

                //}

            //}
            
        //}
        //else
        //{
            //// Move type 3:
            //// Exchange adjacent operand and operator if the resulting expression still a normalized Polish exp.

            //int rand_index = rand() % newSlicingTree.size();        
            
            //if (newSlicingTree[rand_index] >= 0)
            //{
                //if (rand_index + 1 < newSlicingTree.size() and (newSlicingTree[rand_index + 1] < 0))
                //{
                    //if (rand_index - 1 >= 0)
                    //{
                        //std::vector<int> testSlicingTree = newSlicingTree;
                        //int temp = testSlicingTree[rand_index];
                        //testSlicingTree[rand_index] = testSlicingTree[rand_index + 1];
                        //testSlicingTree[rand_index + 1] = temp;

                        //if (isValidSlicingTree(testSlicingTree))
                        //{
                            //newSlicingTree = testSlicingTree;
                            //validMove = true;
                        //}
                    //}
                //}
                //else if (rand_index - 1 >= 0 and (newSlicingTree[rand_index - 1] < 0))
                //{
                    //if (rand_index + 1 < newSlicingTree.size())
                    //{
                        //std::vector<int> testSlicingTree = newSlicingTree;
                        //int temp = testSlicingTree[rand_index];
                        //testSlicingTree[rand_index] = testSlicingTree[rand_index - 1];
                        //testSlicingTree[rand_index - 1] = temp;
                        
                        //if (isValidSlicingTree(testSlicingTree))
                        //{
                            //newSlicingTree = testSlicingTree;
                            //validMove = true;
                        //}
                    //}
                //}
            //}
        //}
    //}

    //return newSlicingTree;
//}

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

//Block scoreSlicingTree(std::vector<int> slicingTree, std::vector<std::vector<Block>> blocks)
//{
    //// Stack for evaluating slicing polish expression
    //std::vector<std::vector <Block>> stack;

    //// Go through all elements in polish slicing tree
    //for (int& i : slicingTree)
    //{
        //if (i == HORIZONTAL_SLICE)
        //{
            //// Stack must be atleast size 2
            //assert(stack.size() >= 2);

            //// Perform horizontal slice
            //std::vector<Block> a = stack.back();
            //stack.pop_back();
            //std::vector<Block> b = stack.back();
            //stack.pop_back();

            //std::vector<Block> comb = horizontalNodeSizing(a, b);

            //// Add result back to stack
            //stack.push_back(comb);
        //}
        //else if (i == VERTICAL_SLICE)
        //{
            //// Stack must be atleast size 2
            //assert(stack.size() >= 2);

            //// Perform vertical slice
            //std::vector<Block> a = stack.back();
            //stack.pop_back();
            //std::vector<Block> b = stack.back();
            //stack.pop_back();

            //std::vector<Block> comb = verticalNodeSizing(a, b);

            //// Add result back to stack
            //stack.push_back(comb);
        //}
        //else
        //{
            //// If not a slice, push blocks to stack
            //stack.push_back(blocks[i]);
        //}
    //}

    //// We have reached the end of the polish expression
    //// There should be only a set of final areas remaining
    ////std::cout << stack.size() << std::endl;
    //assert(stack.size() == 1);

    //// All that is left to do is find the min area
    //int min_area = 1000000000;
    //Block min_area_block;


    //for (Block& b : stack[0])
    //{
        ////std::cout << b << std::endl;
        //if (b.width * b.height < min_area)
        //{
            //min_area = b.width * b.height;
            //min_area_block = b;
        //}
    //}

    ////std::cout << "Min block name: " << min_area_block->blockName << std::endl;

    //return min_area_block;
//}

void sortByWidth(std::vector<Block> &blocks)
{
    std::sort(blocks.begin(), blocks.end(), [](const Block &l, const Block &r){ return l.width < r.width; });
}

void sortByHeight(std::vector<Block> &blocks)
{
    std::sort(blocks.begin(), blocks.end(), [](const Block &l, const Block &r){ return l.height < r.height; });
}

void printBlocks(std::vector<Block> blocks)
{
    for (Block& b : blocks)
    {
        std::cout << b << std::endl;
    }
}

std::vector<Block> verticalNodeSizing(std::vector<Block> a, std::vector<Block> b)
{
    //std::cout << "START VERTICAL SLICE" << std::endl;
    // Sort input sets by width
    sortByWidth(a);
    sortByWidth(b);

    //std::cout << "=================" << std::endl;
    //printBlocks(a);
    //std::cout << "=================" << std::endl;
    //printBlocks(b);
    //std::cout << "=================" << std::endl;

    std::vector<Block> final_blocks;

    int len_a = a.size();
    int len_b = b.size();

    //std::cout << len_a << " " << len_b << std::endl;

    int i = 0, j = 0;
    int new_width, new_height;

    Block current_a, current_b;

    while (i < len_a and j < len_b)
    {
        current_a = a[i];
        current_b = b[j];

        //std::cout << "a " << current_a << std::endl;
        //std::cout << "b " << current_b << std::endl;

        new_width = current_a.width + current_b.width;
        new_height = std::max(current_a.height, current_b.height);


        Block new_block(current_a.blockName + "|" + current_b.blockName, false, new_height, new_width, &current_a, &current_b);

        //std::cout << "new " << new_block << std::endl;

        final_blocks.push_back(new_block);

        if (new_height == current_a.height)
        {
            i++;
        }
        if (new_height == current_b.height)
        {
            j++;
        }
    }


    //std::cout << "END VERTICAL SLICE" << std::endl;
    return final_blocks;
}

std::vector<Block> horizontalNodeSizing(std::vector<Block> a, std::vector<Block> b)
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

    std::vector<Block> final_blocks;

    int len_a = a.size();
    int len_b = b.size();

    //std::cout << len_a << " " << len_b << std::endl;

    int i = 0, j = 0;
    int new_width, new_height;

    Block current_a, current_b;

    while (i < len_a and j < len_b)
    {
        current_a = a[i];
        current_b = b[j];

        //std::cout << "a " << current_a << std::endl;
        //std::cout << "b " << current_b << std::endl;

        new_width = std::max(current_a.width, current_b.width);
        new_height = current_a.height + current_b.height;

        Block new_block(current_a.blockName + "-" + current_b.blockName, false, new_height, new_width, &current_a, &current_b);

        //std::cout << "new " << new_block << std::endl;

        final_blocks.push_back(new_block);

        if (new_width == current_a.width)
        {
            i++;
        }
        if (new_width == current_b.width)
        {
            j++;
        }
    }


    //std::cout << "END HORIZONTAL SLICE" << std::endl;
    return final_blocks;
}

// Check if a slicing tree is valid
//bool isValidSlicingTree(std::vector<int> slicingTree)
//{
    //int count = 0;
    //int previous = 0;

    //for (int& i : slicingTree)
    //{
        //// Check if is a slice
        //if (i < 0)
        //{
            //if (count < 2)
            //{
                //return false;
            //}

            //// Don't allow two of same slice in a row
            //if (i == previous)
            //{
                //return false;
            //}

            //count -= 1;
        //}
        //else
        //{
            //count += 1;
        //}

        //previous = i;
    //}

    //return (count == 1);
    
//}

//void printSlicingTree(std::vector<int> slicingTree)
//{
    //for (int& i : slicingTree)
    //{
        //if (i == HORIZONTAL_SLICE)
        //{
            //std::cout << "-";
        //}
        //else if (i == VERTICAL_SLICE)
        //{
            //std::cout << "|";
        //}
        //else
        //{
            //std::cout << i;
        //}
    //}

    //std::cout << std::endl;
//}
