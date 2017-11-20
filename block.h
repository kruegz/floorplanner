#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <sstream>

class Block;

// Object to store width and height with corresponding indexes in children
class WidthHeight
{
    public:
        // Width and height
        double width;
        double height;

        // Indexes in children
        int left_index;
        int right_index;

        WidthHeight() :
            width(0),
            height(0),
            left_index(0),
            right_index(0)
        {};

        WidthHeight(double width, double height, int left_index, int right_index) :
            width(width),
            height(height),
            left_index(left_index),
            right_index(right_index)
        {};

        // Create a rotated copy
        WidthHeight rotate()
        {
            return WidthHeight(height, width, left_index, right_index);
        }

        double area()
        {
            return width * height;
        }
};

// Object to store a block with all combinations of widths/heights
class Block {
public:

	// Details of the block
    std::string blockName;
	bool isSoft; // true for a soft-macro and null for a hard-macro

    // Set of width/height combinations
    std::vector<WidthHeight> widths_heights;

    int wh_index;

	// To keep track of slicing tree for incremental cost update
    Block *leftChild;
    Block *rightChild;
    Block *parentBlock;

	// For soft-macros only otherwise NULL
	float minAspectRatio;
	float maxAspectRatio;
	float optimalAspectRatio; // the optimal aspect ratio chosen among 1, minAspectRatio and maxAspectRatio

	//To print the coordinates of each block.
	double xCoordinate; // lower left
	double yCoordinate; // lower left

	//Constrcutor to create new objects
    Block() :
        blockName("blank"),
        isSoft(false),
        widths_heights(0),
        wh_index(-1),
        //areas(0),
        leftChild(NULL),
        rightChild(NULL),
        parentBlock(NULL),
        xCoordinate(0),
        yCoordinate(0)
    {};
    
    Block(std::string name) :
        blockName(name),
        isSoft(false),
        widths_heights(0),
        wh_index(-1),
        leftChild(NULL),
        rightChild(NULL),
        parentBlock(NULL),
        xCoordinate(0),
        yCoordinate(0)
    {};

	Block(std::string blockName, bool isSoft, std::vector<WidthHeight> widths_heights) :
        blockName(blockName),
        isSoft(isSoft),
        widths_heights(widths_heights),
        wh_index(-1),
        leftChild(NULL),
        rightChild(NULL),
        parentBlock(NULL),
        xCoordinate(0),
        yCoordinate(0)
    {};
	
    Block(std::string blockName, bool isSoft, std::vector<WidthHeight> widths_heights, Block *lchild, Block *rchild, Block *parent) :
        blockName(blockName),
        isSoft(isSoft),
        widths_heights(widths_heights),
        wh_index(-1),
        leftChild(lchild),
        rightChild(rchild),
        parentBlock(parent),
        xCoordinate(0),
        yCoordinate(0)
    {};


    // Convert to string for debugging
    std::string toString() const
    {
        std::ostringstream stringStream;
        stringStream << "Name: ";
        stringStream << blockName;
        stringStream << " Soft: ";
        stringStream << isSoft;
        stringStream << " Parent: " << parentBlock;
        stringStream << " Left Child: " << leftChild;
        stringStream << " Right Child: " << rightChild;
        stringStream << " wh_index: " << wh_index;
        stringStream << " (widths, heights): ";
        
        for (const WidthHeight & wh : widths_heights)
        {
            stringStream << "(" << wh.width << ", " << wh.height << ") from l: " << wh.left_index << " r: " << wh.right_index << " ";
        }



        return stringStream.str();
    }

    // Output operator
    friend std::ostream &operator<<( std::ostream &output, const Block &b ) {
        output << b.toString();
        return output;
    }

    // Create a rotated version of block
    Block* rotate()
    {
        std::vector<WidthHeight> new_wh = widths_heights;

        for (WidthHeight& wh : new_wh)
        {
            std::swap(wh.width, wh.height);
        }

        return new Block(blockName, isSoft, new_wh);
    }


};

#endif
