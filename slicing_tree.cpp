#include <iostream>
#include <vector>
#include <cassert>
#include <string>

#include "slicing_tree.h"
#include "block.h"
#include "floorplan.h"


// Perform a move
void SlicingTree::makeMove()
{

    bool validMove = false;

    while (not validMove)
    {
        // Perform one of two random moves
        int move_type = rand() & 1;

        if (move_type)
        {
            // Move type 1:
            // Attempt to complement a slice
            int rand_index = rand() % tree.size();

            int old_value = tree[rand_index];

            // Check if random index is a slice
            if (old_value < 0)
            {
                // Complement the slice
                tree[rand_index] = (old_value == HORIZONTAL_SLICE) ? VERTICAL_SLICE : HORIZONTAL_SLICE;

                if (isValid())
                {
                    validMove = true;
                    break;
                }
                else
                {
                    // Fix tree
                    tree[rand_index] = old_value;
                }
            }
        }
        else
        {
            // Move type 2:
            // Swap two elements of the tree (either slices or indexes) as long
            // as valid polish tree results
            int r1 = rand() % tree.size();
            int r2 = rand() % tree.size();

            swap(r1, r2);

            if (isValid())
            {
                validMove = true;
                break;
            }
            else
            {
                // Fix tree
                swap(r1, r2);
            }
        }
    }
}

// Swap two elements in the slicing tree
void SlicingTree::swap(int i, int j)
{
    int tmp = tree[i];
    tree[i] = tree[j];
    tree[j] = tmp;
}

// Check if tree is valid
bool SlicingTree::isValid()
{
    int count = 0;
    int previous = 0;

    for (int& i : tree)
    {
        // Check if is a slice
        if (i < 0)
        {
            if (count < 2)
            {
                return false;
            }

            // Don't allow two of same slice in a row
            if (i == previous)
            {
                return false;
            }

            count -= 1;
            previous = i;
        }
        else
        {
            count += 1;
        }

    }

    return (count == 1);
}

// Convert to string
std::string SlicingTree::toString()
{
    std::string s;

    for (int& i : tree)
    {
        if (i == HORIZONTAL_SLICE)
        {
            s += "-";
        }
        else if (i == VERTICAL_SLICE)
        {
            s += "|";
        }
        else
        {
            s += std::to_string(i);
        }
    }

    return s;
}

// Find minimum area
Block SlicingTree::score()
{
    // Stack for evaluating slicing polish expression
    std::vector<std::vector <Block>> stack;

    // Go through all elements in polish slicing tree
    for (int& i : tree)
    {
        if (i == HORIZONTAL_SLICE)
        {
            // Stack must be atleast size 2
            assert(stack.size() >= 2);

            // Perform horizontal slice
            std::vector<Block> a = stack.back();
            stack.pop_back();
            std::vector<Block> b = stack.back();
            stack.pop_back();

            std::vector<Block> comb = horizontalNodeSizing(a, b);

            // Add result back to stack
            stack.push_back(comb);
        }
        else if (i == VERTICAL_SLICE)
        {
            // Stack must be atleast size 2
            assert(stack.size() >= 2);

            // Perform vertical slice
            std::vector<Block> a = stack.back();
            stack.pop_back();
            std::vector<Block> b = stack.back();
            stack.pop_back();

            std::vector<Block> comb = verticalNodeSizing(a, b);

            // Add result back to stack
            stack.push_back(comb);
        }
        else
        {
            // If not a slice, push blocks to stack
            stack.push_back(blocks[i]);
        }
    }

    // We have reached the end of the polish expression
    // There should be only a set of final areas remaining
    //std::cout << stack.size() << std::endl;
    assert(stack.size() == 1);

    // All that is left to do is find the min area
    int min_area = 1000000000;
    Block min_area_block;


    for (Block& b : stack[0])
    {
        //std::cout << b << std::endl;
        if (b.width * b.height < min_area)
        {
            min_area = b.width * b.height;
            min_area_block = b;
        }
    }

    //std::cout << "Min block name: " << min_area_block->blockName << std::endl;

    return min_area_block;
}
