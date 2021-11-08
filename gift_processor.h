/********************************************
 * processor.h
 *
 * Brett Huffman
 * CMP SCI 5130 - Gifting Project
 * Due Nov 19, 2021
 * processor.h file for processing the
 * how gifts get matched to children
 ********************************************/
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <set>
#include <string>
#include "gift_main.h"

using namespace std;

// giftProcess - Process to start the match process
int giftProcess(string, string);

// loads the struct arrays from input file
bool loadArraysFromFile(string);
bool loadLogicTable();
bool isLogicMatch(int, int);
void printLogicTable();
void deleteLogicTable();
bool populateTreeAndPrune();
void processChild(const int, const vector<int>&);

#endif // PROCESSOR_H