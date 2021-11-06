/********************************************
 * Gifting Project - Main cpp file
 * Brett Huffman
 * CMP SCI 5130 - Project 1
 * Due Nov 19, 2021
 * Main CPP file for project
 ********************************************/
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <cerrno>
#include <fstream>
#include "processor.h"
#include "main.h"

using namespace std;

string strInputFilePath;
string strOutputFilePath;


// Handle errors in input arguments by showing usage screen
static void show_usage(std::string name)
{
    std::cerr << "\ngifting - Gifting application by Brett Huffman for CMP SCI 5130\n"
              << "Usage: " << name << " inputFileName outputFileName\n"
              << "Options:\n"
              << "inputFileName - the file containing child and gift info\n"
              << "outputFileName - the file to write the optimum gifting distribution."
              << std::endl << std::endl;
}

// Main - expecting arguments
int main(int argc, char* argv[])
{
    // Argument processing
    int opt;

    // Must return an input and output file
    if(argc < 3)
    {
        show_usage(argv[0]);
        return 0;
    }

    // Get the two files
    strInputFilePath = argv[1];
    strOutputFilePath = argv[2];

    // Call the giftProcess, which processes the gifting logic
    return giftProcess(strInputFilePath, strOutputFilePath);
}
