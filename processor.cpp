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

// Constants
const float GIFTSIZE_MED_LOWER = 1.0;
const float GIFTSIZE_MED_UPPER = 2.0;
const float GIFTSIZE_LRG_LOWER = 2.0;
const float GIFTSIZE_LRG_UPPER = 100.0;

// The two main vectors gifts and children
vector<Gift> vecGiftItems;      // Vector of all Gifts
vector<Child> vecChildren;    // Vector of all children
vector<vector<Node> > vecChildBranches; // Vec of Vec of Nodes for analysis
vector<int>  vecMedGifts;       // Nodes pointing to Medium Gifts
vector<int>  vecLrgGifts;       // Nodes pointing to Large Gifts
int** childGiftLogicTable;      // Fast lookup table
int nChildCount = 0;
int nGiftCount = 0;

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
    // Load the logic table from this
    if(!loadLogicTable())
    {
        // Trouble loading and parsing input file -- exit with error
        perror("Could not load logic table");
        exit(EXIT_FAILURE);
    }

    // Debug print logic
    printLogicTable();

    // Build the logic trees & prune
    if(!populateMinTreeAndPrune())
    {
        // Trouble loading and parsing input file -- exit with error
        perror("An unknown error occured while populating the logic tree");
        cout << endl << "Cleaning Up" << endl;
        deleteLogicTable();
        exit(EXIT_FAILURE);
    }

    // Analyze each node looking for the lowest cost



    // Clean up
    cout << endl << "Cleaning Up" << endl;
    deleteLogicTable();

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
            vecChildren.push_back(c);
        }
        else
        {
            // Make a child object and fill
            Gift g;
            ss >> g.giftName;
            ss >> g.price;
            ss >> g.volume;
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
            // Handle gift sizes
            // Only a real gift if it's at least a medium size
            if(g.volume >= GIFTSIZE_MED_LOWER)
            {
                vecGiftItems.push_back(g);
                int loc = vecGiftItems.size()-1;
                if(g.volume < GIFTSIZE_LRG_LOWER)
                    vecMedGifts.push_back(loc);
                else
                    vecLrgGifts.push_back(loc);
            }
        }
        // Reset the string stream
        ss.clear();

        std::cout << line << std::endl;
    }

    infile.close();
    return true;
}

bool loadLogicTable()
{
    nChildCount = vecChildren.size();
    nGiftCount = vecGiftItems.size();

    if(nChildCount > 0 && nGiftCount > 0)
    {

        childGiftLogicTable = new int*[nChildCount];
        for(int i = 0; i < nChildCount; ++i)
        {
            childGiftLogicTable[i] = new int[nGiftCount];

            // Now go through each child/gift and fill in table
            for(int j = 0; j < nGiftCount; j++)
            {
                if(vecChildren[i].age >= vecGiftItems[j].ageMin 
                  && vecChildren[i].age <= vecGiftItems[j].ageMax)
                    childGiftLogicTable[i][j] = 1;
                else
                    childGiftLogicTable[i][j] = 0;
            }
        }
        return true;
    }
    return false;
}

bool isLogicMatch(int child, int gift)
{
    return childGiftLogicTable[child][gift];
}

void printLogicTable()
{
    for(int i = 0; i < nChildCount; ++i)
    {
        for(int j = 0; j < nGiftCount; j++)
            cout << childGiftLogicTable[i][j];
        cout << endl;
    }
}

void deleteLogicTable()
{
    // Delete the first set of columns
    for(int i = 0; i < nChildCount; ++i)
        delete[] childGiftLogicTable[i];
    // Delete the rest of the rows
    delete[] childGiftLogicTable;
}

bool populateMinTreeAndPrune()
{
    // Child 1 & every gift combo 1 gets a vecChildNodes 
    // One child + 1 med and 1 large
    int medGiftStart = 0;
    int lrgGiftStart = 0;

    // Use the min of the med and large gifts.  This will be the
    // ultimate size of the tree children^min(gifts)
    int giftsMax = min(vecMedGifts.size(), vecLrgGifts.size());

    // Iterate through each gift * gift branches
    for(int b = 0; 
        b < vecChildren.size()^giftsMax; b++)
    {
        int medCurrent = medGiftStart;
        int lrgCurrent = lrgGiftStart;

        // Bool var to determine if this branch is good
        bool bIsGoodBranch = true;
        vector<Node> branch;

        // Go through children
        for(int c = 0; 
            bIsGoodBranch && c < vecChildren.size(); c++)
        {

            // Check this node will fit all requirements
            // for child age
            if(!isLogicMatch(c, vecMedGifts[medCurrent]) 
                || !isLogicMatch(c, vecLrgGifts[lrgCurrent]))
            {
                bIsGoodBranch = false;
                break;
            }   

            // Create the node
            Node n;

            // Add Child & Gifts to Node
            n.child = c;
            n.giftMed = medCurrent;
            n.giftLarge = lrgCurrent;
            // put it in the branch
            b.push_back(n);

            // Increment everything
            medCurrent++;
            lrgCurrent++;
        }
        if(bIsGoodBranch)
            vecChildBranches.push_back(branch);
        
        // Increment everything
        medGiftStart++;
        lrgGiftStart++;
    }
    return true;
}