#ifndef FLOORPLAN_H
#define FLOORPLAN_H

#include <vector>

#include "block.h"

#define HORIZONTAL_SLICE -1
#define VERTICAL_SLICE -2

#define STARTING_TEMP 1000000
#define FREEZING_TEMP 10

#define NUM_MOVES_PER_STEP 50
#define COOLING_FACTOR 0.99

#define boltz_k 1

// Perform floor planning using simulated annealing
std::vector<Block> floorplan(std::vector<Block> blocks);

// Make a valid move to a slicing tree for simulated annealing
std::vector<int> makeMove(std::vector<int> slicingTree);

// Determine if move is acceptable
bool acceptMove(int cost, int temp);

// Score a slicing tree
Block scoreSlicingTree(std::vector<int> slicingTree, std::vector<std::vector <Block>> blocks);

// Size a vertical slice of two sets of blocks
void verticalNodeSizing(Block* a, Block* b, Block *result);

// Size a horizontal slice of two sets of blocks
void horizontalNodeSizing(Block* a, Block* b, Block *result);

// Functions to sort Block objects
void sortByHeight(Block *b);
void sortByWidth(Block *b);

// Check if a slicing tree is valid
bool isValidSlicingTree(std::vector<int> slicingTree);

void printSlicingTree(std::vector<int> slicingTree);

void deleteSuperBlock(Block *b);

#endif
