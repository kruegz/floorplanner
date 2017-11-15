#ifndef SLICING_TREE_H
#define SLICING_TREE_H

#include "block.h"

class SlicingTree
{
    public:

        // The tree as indexes and slices
        std::vector<int> tree;

        // The blocks of the tree
        std::vector<std::vector<Block>> blocks;

        
        // Default constructor
        SlicingTree() :
            tree(0),
            blocks(0)
        {};
        
        // Constructor
        SlicingTree(std::vector<int> tree, std::vector<std::vector <Block>> blocks) :
            tree(tree),
            blocks(blocks)
        {};

        // Perform a move
        void makeMove();

        // Check if tree is valid
        bool isValid();

        // Swap two elements in the slicing tree
        void swap(int i, int j);

        // Convert to string
        std::string toString();

        // Find minimum area
        Block score();
};


#endif
