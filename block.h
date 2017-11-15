#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <sstream>

class Block {
public:

	// Details of the block
    std::string blockName;
	bool isSoft; // true for a soft-macro and null for a hard-macro
	float width;
	float height;
	float area;

	// To keep track of slicing tree for incremental cost update
	Block* parentBlock;
	Block* leftChild;
	Block* rightChild;

	// For soft-macros only otherwise NULL
	float minAspectRatio;
	float maxAspectRatio;
	float optimalAspectRatio; // the optimal aspect ratio chosen among 1, minAspectRatio and maxAspectRatio

	//To print the coordinates of each block.
	float xCoordinate; // lower left
	float yCoordinate; // lower left

	//Constrcutor to create new objects
    Block() :
        blockName("blank"),
        isSoft(false),
        height(0),
        width(0),
        area(0)
    {};

	Block(std::string blockName, bool isSoft, float height, float width) :
        blockName(blockName),
        isSoft(isSoft),
        width(width),
        height(height)
    {};
	
    Block(std::string blockName, bool isSoft, float height, float width, Block *leftChild, Block* rightChild) :
        blockName(blockName),
        isSoft(isSoft),
        width(width),
        height(height),
        leftChild(leftChild),
        rightChild(rightChild)
    {};

    std::string toString() const
    {
        std::ostringstream stringStream;
        stringStream << "Name: ";
        stringStream << blockName;
        stringStream << " Soft: ";
        stringStream << isSoft;
        stringStream << " Width: ";
        stringStream << width;
        stringStream << " Height: ";
        stringStream << height;
        std::string copyOfStr = stringStream.str();

        return stringStream.str();
    }

    friend std::ostream &operator<<( std::ostream &output, const Block &b ) {
        output << b.toString();
        return output;
    }

    Block rotate()
    {
        return Block(blockName, isSoft, width, height);
    }

};

#endif
