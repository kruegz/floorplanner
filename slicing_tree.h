#ifndef SLICING_TREE_H
#define SLICING_TREE_H

#include "block.h"

class SlicingTree
{
    public:

        // The blocks of the tree
        std::vector<Block*> blocks;

        
        // Default constructor
        SlicingTree() :
            blocks(0)
        {};
        
        // Constructor
        SlicingTree(std::vector<Block*> blocks) :
            blocks(blocks)
        {};

        // Destructor
        ~SlicingTree();
        
        SlicingTree(std::vector<Block> blocks);

        // Perform a move
        double makeMove();

        // Check if tree is valid
        bool isValid();

        // Swap two elements in the slicing tree
        void swap(int i, int j);

        // Convert to string
        std::string toString();

        // Score a single block
        void scoreSingle(Block *b);
        
        // Score a block and all parents recursively
        double scoreUp(Block *b);

        // Compute areas
        double score();

        // Overload assignment
        SlicingTree& operator= (const SlicingTree &s);

        // Recursively compute coordinates
        void computeCoords(Block *b, int wh_index);

        // Get coordinates
        std::string getCoords();

        void fixPointers();

};


#endif
