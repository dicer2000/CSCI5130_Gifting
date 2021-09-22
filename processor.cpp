/********************************************
 * Gifting Project - Processor cpp file
 * Brett Huffman
 * CMP SCI 5130 - Project 1
 * Due Oct 27, 2021
 * Main file for processing 
 * how gifts get matched to children
 ********************************************/
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <cerrno>
#include <fstream>
#include <sstream>
#include "processor.h"
#include <filesystem>

using namespace std;

using filesystem::current_path;

// The two main vectors gifts and children
vector<Gift> vecGiftItems;
vector<Child> vecChildItems;


// giftProcess - Process to start the match process
int giftProcess(const string inputFile, const string outputFile)
{

    // Try to open the file and parse it in
    if(!loadArraysFromFile(inputFile))
    {
        // Trouble loading and parsing input file -- exit with error
        perror("Could not open or parse the input file");
        exit(EXIT_FAILURE);
    }


    return true;
}

// loads the struct arrays from input file
bool loadArraysFromFile(const string inputFile)
{

// Just for debugging now
current_path("/Users/brett/Documents/Personal/PhD/2021_Algorithms/Projects/GiftingProject");
cout << "Current working directory: " << current_path() << endl;

    ifstream infile(inputFile);
    stringstream ss;
    string temp;


    if (!infile.is_open())
        return false;
    
    // Keep track of what is being parsed in file
    bool bInChild = true;

    // Read in each line
    // Parse the file
    for (string line; getline(infile, line); ) 
    {
        // Remove blank lines
        if(line.empty())
            continue;

        // See the gift header, if so, switch to gifts
        if(line.rfind("Gifts", 0) == 0)
        {
            bInChild = false;
            continue;
        }

        // Get next line to parse
        ss << line;

        // Child parsing is different than gift
        if(bInChild)
        {
            // Make a child object and fill
            Child c;
            ss >> c.childName;
            ss >> temp;
            ss >> c.age;
            vecChildItems.push_back(c);
        }
        else
        {
            // Make a child object and fill
            Gift g;
            ss >> g.giftName;
            ss >> g.price;
            ss >> g.size;
            g.isAgeDependent = false;

            // Is age dependent?
            ss >> temp;
            
            if(temp=="any")
            {
                g.ageMin = 0;
                g.ageMax = 1000;
            }
            else
            {
                // Get position of dash and parse
                int pos = temp.find("-", 0);
                if(pos > 0)
                {
                    string str = temp.substr(0, pos);
                    g.ageMin = atoi(str.c_str());
                    str = temp.substr(pos+1);
                    g.ageMax = atoi(str.c_str());
                    g.isAgeDependent = true;
                }
            }
            
            vecGiftItems.push_back(g);
        }
        // Reset the string stream
        ss.clear();

        std::cout << line << std::endl;
    }

    infile.close();
    return true;
}