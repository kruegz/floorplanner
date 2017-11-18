#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <limits>
#include <unistd.h>

#include "slicing_tree.h"
#include "block.h"
#include "floorplan.h"



SlicingTree::SlicingTree(std::vector<Block> blks)
{
    for (Block &b : blks)
    {
        Block *new_b = new Block;
        *new_b = b;
        blocks.push_back(new_b);
    }

    for (int i = 0; i < blks.size() - 1; i++)
    {
        if (i & 1)
        {
            blocks.push_back(new Block("|"));
        }
        else
        {
            blocks.push_back(new Block("-"));
        }
    }
}

// Perform a move and return the new score
double SlicingTree::makeMove()
{
    double new_score;
    bool validMove = false;

    while (not validMove)
    {
        // Perform one of two random moves
        int move_type = rand() & 1;
        //std::cout << "Move type: " << move_type << std::endl;
        //usleep(100000);

        if (move_type)
        {
            // Move type 1:
            // Attempt to complement a slice
            int rand_index = rand() % blocks.size();

            //int old_value = tree[rand_index];
            Block *b = blocks[rand_index];

            //std::cout << "Rand index: " << rand_index << std::endl;

            // Check if random index is a slice
            if (b->blockName == "|")
            {
                // Complement the slice
                b->blockName = "-";

                //std::cout << toString() << std::endl;

                if (isValid())
                {
                    // Calculate new scores
                    //std::cout << "before scoreup" << std::endl;
                    new_score = score();
                    //std::cout << "after scoreup" << std::endl;

                    validMove = true;
                }
                else
                {
                    // Fix tree
                    b->blockName = "|";
                }
            }
            else if (b->blockName == "-")
            {
                // Complement the slice
                b->blockName = "|";

                //std::cout << toString() << std::endl;

                if (isValid())
                {
                    // Calculate new scores
                    //std::cout << "before scoreup" << std::endl;
                    new_score = score();
                    //std::cout << "after scoreup" << std::endl;

                    validMove = true;
                }
                else
                {
                    // Fix tree
                    b->blockName = "-";
                }
            }
        }
        else
        {
            // Move type 2:
            // Swap two elements of the tree (either slices or indexes) as long
            // as valid polish tree results
            int r1 = rand() % blocks.size();
            int r2 = rand() % blocks.size();

            // Make sure indexes are not the same
            while (r2 == r1)
            {
                r2 = rand() % blocks.size();
            }
            
            //std::cout << "r1 r2 " << r1 << " " << r2 << std::endl;

            //Block tmp = *blocks[r1];
            //*blocks[r1] = *blocks[r2];
            //*blocks[r2] = tmp;
            std::swap(blocks[r1], blocks[r2]);
                
            //std::cout << toString() << std::endl;

            if (isValid())
            {
                // Score from bottom
                new_score = score();
                validMove = true;
            }
            else
            {
                // Fix tree
                //Block tmp = *blocks[r1];
                //*blocks[r1] = *blocks[r2];
                //*blocks[r2] = tmp;
                std::swap(blocks[r1], blocks[r2]);
            }
        }
    }

    return new_score;
}

// Swap two elements in the slicing tree
//void SlicingTree::swap(int i, int j)
//{
    //int tmp = tree[i];
    //tree[i] = tree[j];
    //tree[j] = tmp;
//}

// Check if tree is valid
bool SlicingTree::isValid()
{
    int count = 0;
    std::string prev_name;
    std::string new_name;

    for (Block *b : blocks)
    {
        // Check if is a slice
        new_name = b->blockName;

        if (new_name == "|" or new_name == "-")
        {
            if (count < 2)
            {
                return false;
            }

            // Don't allow two of same slice in a row
            if (new_name == prev_name)
            {
                return false;
            }

            count -= 1;
        }
        else
        {
            count += 1;
        }
        
        prev_name = new_name;

    }

    return (count == 1);
}

// Convert to string
std::string SlicingTree::toString()
{
    std::stringstream str;

    for (Block *b : blocks)
    {
        str << b->blockName << " ";
    }

    return str.str();
}

void printVector(std::vector<Block*> blocks)
{
    for (Block *b : blocks)
    {
        std::cout << *b << std::endl;
    }
}

//Block * copySuperBlock(Block *b)
//{
    //if (b != NULL)
    //{
        //Block *new_block = new Block;
        //*new_block = b;
        //new_block->leftChild = copySuperBlock(b->leftChild);
        //new_block->rightChild = copySuperBlock(b->rightChild);
    //}
    //else
    //{
        //return NULL;
    //}

//}

void SlicingTree::scoreSingle(Block *b)
{
    if (b->blockName == "|")
    {
        assert(b->leftChild != NULL);
        assert(b->rightChild != NULL);

        verticalNodeSizing(b->leftChild, b->rightChild, b);
    }
    else if (b->blockName == "-")
    {
        assert(b->leftChild != NULL);
        assert(b->rightChild != NULL);

        horizontalNodeSizing(b->leftChild, b->rightChild, b);
    }
}

// Score the current block and all parents recursively
// Return the new min area
double SlicingTree::scoreUp(Block *b)
{
    scoreSingle(b);

    // Check if we have a parent
    if (b->parentBlock != NULL)
    {
        // Return the parent score
        return scoreUp(b->parentBlock);
    }
    else
    {
        // We are root
        // Return min area
        double min_area = std::numeric_limits<double>::max();

        assert(b->widths_heights.size() >= 1);

        for (WidthHeight &wh : b->widths_heights)
        {
            double area = wh.width * wh.height;

            if (area < min_area)
            {
                min_area = area;
            }
        }

        assert(min_area != std::numeric_limits<double>::max());
        
        return min_area;
    }
}

// Find minimum area
// Return index of best area in root block
double SlicingTree::score()
{
    // Stack for evaluating slicing polish expression
    std::vector<Block*> stack;

    // Go through all elements in polish slicing tree
    for (Block *i : blocks)
    {
        //std::cout << i->blockName << std::endl;
        if (i->blockName == "-")
        {
            // Stack must be atleast size 2
            assert(stack.size() >= 2);

            // Perform horizontal slice
            Block *a = stack.back();
            stack.pop_back();
            Block *b = stack.back();
            stack.pop_back();

            // Mark parent
            a->parentBlock = i;
            b->parentBlock = i;
            i->leftChild = a;
            i->rightChild = b;

            // Compute sizes storing in i
            horizontalNodeSizing(a, b, i);

            // Add result back to stack
            stack.push_back(i);
        }
        else if (i->blockName == "|")
        {
            // Stack must be atleast size 2
            assert(stack.size() >= 2);

            // Perform vertical slice
            Block *a = stack.back();
            stack.pop_back();
            Block *b = stack.back();
            stack.pop_back();
            
            // Mark parent
            a->parentBlock = i;
            b->parentBlock = i;
            i->leftChild = a;
            i->rightChild = b;


            // Compute sizes storing in i
            verticalNodeSizing(a, b, i);


            // Add result back to stack
            stack.push_back(i);
        }
        else
        {
            // If not a slice, push blocks to stack
            stack.push_back(i);
        }
    }

    // We have reached the end of the polish expression
    // There should be only a set of final areas remaining
    //std::cout << stack.size() << std::endl;
    assert(stack.size() == 1);

    // All that is left to do is find the min area
    double min_area = std::numeric_limits<double>::max();
    int min_area_index = -1;

    assert(stack[0]->widths_heights.size() >= 1);


    for (int i = 0; i < stack[0]->widths_heights.size(); i++)
    {
        //std::cout << *b << std::endl;
        double new_area = stack[0]->widths_heights[i].width * stack[0]->widths_heights[i].height;

        if (new_area < min_area)
        {
            min_area = new_area;
            min_area_index = i;
        }
    }

    assert(min_area_index != -1);

    //std::cout << "Min block name: " << min_area_block->blockName << std::endl;

    return min_area;
}
        
SlicingTree& SlicingTree::operator= (const SlicingTree &s)
{
    std::vector<Block*> new_blocks;

    for (Block *b : s.blocks)
    {
        Block *new_b = new Block;
        *new_b = *b;
        new_blocks.push_back(new_b);
        //std::cout << "orig b" << *b << std::endl;
        //std::cout << "new b" << *new_b << std::endl;
    }

    blocks = new_blocks;

    return *this;
}

SlicingTree::~SlicingTree()
{
    for (Block *b : blocks)
    {
        delete b;
    }
}
        
// Get coordinates
std::string SlicingTree::getCoords()
{
    std::string s;
    return s;
    
}
