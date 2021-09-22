/********************************************
 * processor.h
 *
 * Brett Huffman
 * CMP SCI 5130 - Gifting Project
 * Due Oct 27, 2021
 * processor.h file for processing the
 * how gifts get matched to children
 ********************************************/
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include "main.h"

using namespace std;

// giftProcess - Process to start the match process
int giftProcess(string, string);

// loads the struct arrays from input file
bool loadArraysFromFile(string);




#endif // PROCESSOR_H