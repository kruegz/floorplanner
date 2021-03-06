#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <limits>
#include <unistd.h>

#include "slicing_tree.h"
#include "block.h"
#include "floorplan.h"

// Construct a slicing tree from a set of blocks
SlicingTree::SlicingTree(std::vector<Block> blks)
{
    // Add blocks to tree
    for (Block &b : blks)
    {
        Block *new_b = new Block;
        *new_b = b;
        blocks.push_back(new_b);
    }
    
    // Add alternating horizontal and vertical slices
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

// Fix the parent/child relationships in a slicing tree
void SlicingTree::fixPointers()
{
    // Stack for evaluating slicing polish expression
    std::vector<Block*> stack;

    // Go through all elements in polish slicing tree
    for (Block *i : blocks)
    {
        // Check if slice
        if (i->blockName == "-" or i->blockName == "|")
        {
            // Stack must be atleast size 2
            assert(stack.size() >= 2);

            // Get children
            Block *a = stack.back();
            stack.pop_back();
            Block *b = stack.back();
            stack.pop_back();
            
            // Mark parent/child
            a->parentBlock = i;
            b->parentBlock = i;
            i->leftChild = a;
            i->rightChild = b;
        }

        // Add result back to stack
        stack.push_back(i);
    }

    assert(stack.size() == 1);

    // Fix root node parent
    stack[0]->parentBlock = NULL;

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

        if (move_type)
        {
            // Move type 1:
            // Attempt to complement a slice

            int rand_index = rand() % blocks.size();

            Block *b = blocks[rand_index];

            std::string bname = b->blockName;

            // Check if random index is a slice
            if (bname == "|" or bname == "-")
            {
                // Complement the slice
                b->blockName = (bname == "|") ? "-" : "|";

                // Check if resulting slicing tree is valid polish
                if (isValid())
                {
                    // Calculate new score
                    new_score = scoreUp(b);

                    validMove = true;
                }
                else
                {
                    // Fix tree
                    b->blockName = bname;
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

            Block *b1 = blocks[r1];
            Block *b2 = blocks[r2];
            
            // Swap the blocks
            std::swap(blocks[r1], blocks[r2]);


            if (isValid())
            {
                // Score from bottom
                fixPointers();

                new_score = scoreUp(b1);
                new_score = scoreUp(b2);

                validMove = true;
                //std::cout << "VALID" << std::endl;
            }
            else
            {
                // Fix tree
                std::swap(blocks[r1], blocks[r2]);
                fixPointers();

            }

        }
    }

    return new_score;
}


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


// Score a single block 
void SlicingTree::scoreSingle(Block *b)
{
    // Check if slice
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
    // Score the current block
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

        // Loop through all possible width/height combinations for current block
        for (WidthHeight &wh : b->widths_heights)
        {
            double area = wh.area();

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
    assert(stack.size() == 1);

    // All that is left to do is find the min area
    double min_area = std::numeric_limits<double>::max();
    int min_area_index = -1;

    assert(stack[0]->widths_heights.size() >= 1);


    for (int i = 0; i < stack[0]->widths_heights.size(); i++)
    {
        double new_area = stack[0]->widths_heights[i].area();

        if (new_area < min_area)
        {
            min_area = new_area;
            min_area_index = i;
        }
    }

    assert(min_area_index != -1);


    return min_area;
}

// Slicing tree assignment
SlicingTree& SlicingTree::operator= (const SlicingTree &s)
{

    // Clear old blocks
    for (Block *b : blocks)
    {
        delete b;
    }

    std::vector<Block*> new_blocks;

    // Make copies of each block
    for (Block *b : s.blocks)
    {
        Block *new_b = new Block;
        *new_b = *b;
        new_blocks.push_back(new_b);
    }

    blocks = new_blocks;

    fixPointers();

    return *this;
}

// Clear memory for a slicing tree
SlicingTree::~SlicingTree()
{
    // Delete all the blocks
    for (Block *b : blocks)
    {
        if (b != NULL)
        {
            delete b;
        }
    }
}

// Recursively compute coordinates
void SlicingTree::computeCoords(Block *b, int wh_index)
{
    // Check if slice
    if (b->blockName == "|")
    {
        assert(b->leftChild != NULL);
        assert(b->rightChild != NULL);

        // Get specific indexes in children
        int left_child_index = b->widths_heights[wh_index].left_index;
        int right_child_index = b->widths_heights[wh_index].right_index;

        // Set child indexes
        b->leftChild->wh_index = left_child_index;
        b->rightChild->wh_index = right_child_index;

        // Get left child width
        double left_width = b->leftChild->widths_heights[left_child_index].width;

        // Set left child coords
        b->leftChild->xCoordinate = b->xCoordinate;
        b->leftChild->yCoordinate = b->yCoordinate;
        
        // Set right child coords
        b->rightChild->xCoordinate = b->xCoordinate + left_width;
        b->rightChild->yCoordinate = b->yCoordinate;

        // Recurse
        computeCoords(b->leftChild, left_child_index);
        computeCoords(b->rightChild, right_child_index);

    }
    else if (b->blockName == "-")
    {
        assert(b->leftChild != NULL);
        assert(b->rightChild != NULL);

        // Get specific indexes in children
        int left_child_index = b->widths_heights[wh_index].left_index;
        int right_child_index = b->widths_heights[wh_index].right_index;

        // Set child indexes
        b->leftChild->wh_index = left_child_index;
        b->rightChild->wh_index = right_child_index;

        // Get left child height
        double left_height = b->leftChild->widths_heights[left_child_index].height;

        // Set left child coords
        b->leftChild->xCoordinate = b->xCoordinate;
        b->leftChild->yCoordinate = b->yCoordinate;
        
        // Set right child coords
        b->rightChild->xCoordinate = b->xCoordinate;
        b->rightChild->yCoordinate = b->yCoordinate + left_height;

        // Recurse
        computeCoords(b->leftChild, left_child_index);
        computeCoords(b->rightChild, right_child_index);
    }
    else
    {
        // Leaf node
        assert(b->wh_index != -1);
    }
}
        
// Get coordinates
std::string SlicingTree::getCoords()
{
    std::stringstream s;

    // Find min area index
    Block *root = blocks[blocks.size() - 1];

    assert(root->blockName == "|" or root->blockName == "-");

    // Find the minimum area combination of width/height for root
    int min_index = -1;
    double min_area = std::numeric_limits<double>::max();

    assert(root->widths_heights.size() >= 1);

    for (int i = 0; i < root->widths_heights.size(); i++)
    {
        if (root->widths_heights[i].area() < min_area)
        {
            min_area = root->widths_heights[i].area();
            min_index = i;
        }
    }

    assert(min_index != -1);

    // Compute coords
    computeCoords(root, min_index);


    // Print coords
    for (Block *b : blocks)
    {
        if (b->blockName != "|" and b->blockName != "-")
        {
            assert(b->wh_index != -1);

            double sx = b->xCoordinate;
            double sy = b->yCoordinate;
            double ex = b->xCoordinate + b->widths_heights[b->wh_index].width;
            double ey = b->yCoordinate + b->widths_heights[b->wh_index].height;

            s << b->blockName << " (" << sx << ", " << sy << ") (" << ex << ", " << ey << ")" << std::endl;
        }
    }
    

    return s.str();
}
