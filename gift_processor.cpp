/********************************************
 * Gifting Project - Processor cpp file
 * Brett Huffman
 * CMP SCI 5130 - Project 1
 * Due Nov 19, 2021
 * Main file for processing 
 * how gifts get matched to children
 ********************************************/
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unistd.h>
#include <cerrno>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <set>
#include <iomanip> // For Precision output
#include "gift_processor.h"
#include "utils.h"

using namespace std;
using std::__fs::filesystem::current_path;
//using std::filesystem::current_path;

// Constants
const float GIFTSIZE_MED_LOWER = 1.0;
const float GIFTSIZE_LRG_LOWER = 2.0;

// The two main vectors gifts and children
vector<Gift> vecGiftItems;      // Vector of all Gifts
vector<Child> vecChildren;    // Vector of all children
// Additional objects to make life easier
ChildNode *startChild;
vector<int>  vecMedGifts;       // Nodes pointing to Medium Gifts
vector<int>  vecLrgGifts;       // Nodes pointing to Large Gifts
// Objects to keep track of the best found combos
vector<vector<int> > vecCurrentGiftCombos; // Vec of Vec of Gift Combos
vector<vector<int> > vecBestGiftCombos; // Vec of Vec of Gift Combos
float bestFoundAvg =  numeric_limits<float>::max();
float calculatedGiftP_N = 0.0;
// Fast lookup table
int** childGiftLogicTable;
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

    // Calculate the Gift P/N
    if(vecGiftItems.size() > 0)
    {
        float fTempP = 0.0;
        for(int i = 0; i < vecGiftItems.size(); i++)
            fTempP += vecGiftItems[i].price;
        calculatedGiftP_N = fTempP / (float)vecChildren.size();
    }
    else
    {
        // Trouble loading and parsing input file -- exit with error
        perror("Error populating the gift logic -- no gifts found");
        cout << endl << "Cleaning Up" << endl;
        deleteLogicTable();
        exit(EXIT_FAILURE);
    }

    // Get the best path vectors ready
    if(vecChildren.size() > 0)
    {
        vecBestGiftCombos.resize(vecChildren.size());
        vecCurrentGiftCombos.resize(vecChildren.size());
    }
    else
    {
        // Trouble loading and parsing input file -- exit with error
        perror("Error populating the child logic -- no children found");
        cout << endl << "Cleaning Up" << endl;
        deleteLogicTable();
        exit(EXIT_FAILURE);
    }

    // Build the logic trees & prune
    if(calculatedGiftP_N == 0.0 || !populateTreeAndPrune())
    {
        // Trouble loading and parsing input file -- exit with error
        perror("An unknown error occured while populating the logic tree");
        cout << endl << "Cleaning Up" << endl;
        deleteLogicTable();
        exit(EXIT_FAILURE);
    }

    // print out the Sum_e_i and best path combo
    cout << "Sum_e_i "  << std::setprecision(7) << bestFoundAvg << endl;
    for(int i = 0; i < vecBestGiftCombos.size(); ++i)
    {
        // Print each child and their gift combo
        cout << "Child" << (i+1);
        for(int j = 0; j < vecBestGiftCombos[i].size(); j++)
            cout << " " << vecGiftItems[vecBestGiftCombos[i][j]].giftName;
        cout << endl;
    }

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
//cout << "Current working directory: " << current_path() << endl;

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

//        std::cout << line << std::endl;
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

bool populateTreeAndPrune()
{
    // Check for problems before we start
    if(childGiftLogicTable == NULL ||
        vecChildren.size() < 1 ||
        vecGiftItems.size() < 1)
        return false;

    vector<int> vecUsedGifts;
    processChild(0, vecUsedGifts);
    
    return true;
}

void processChild(const int nCurrentChild, const vector<int>& vecUsedGifts)
{
    cout << "*** Child " << nCurrentChild << " ***" << endl;

    int nNewChild = nCurrentChild + 1;

    // If no child, then *success* - calc path cost
    if(nNewChild > vecChildren.size())
    {
        cout << "### Calculating Path Summary: " << bestFoundAvg << endl;

        // Calculate the cost of this path
        // if better than best found so far,
        // make this the winner
        float fGrandTotal = 0.0f;
        float fChildTotal = 0.0f;
        for(int i = 0; i < vecCurrentGiftCombos.size(); i++)
        {
            cout << "  c" << i << ": ";
            fChildTotal = 0;
            for(int j = 0; j < vecCurrentGiftCombos[i].size(); j++)
            {
                fChildTotal += vecGiftItems[vecCurrentGiftCombos[i][j]].price;
                
                cout << vecGiftItems[vecCurrentGiftCombos[i][j]].giftName << "/" << vecGiftItems[vecCurrentGiftCombos[i][j]].price << " ";
            }
            // subtract the child total from the P/N
            fChildTotal = std::abs(fChildTotal - calculatedGiftP_N);
            cout << "=>" << calculatedGiftP_N << ":" << fChildTotal << endl;
            fGrandTotal += fChildTotal;
        }
        // If the grand total is less than the best so far,
        // Make this path the winner
        if(fGrandTotal < bestFoundAvg)
        {
            bestFoundAvg = fGrandTotal;
            vecBestGiftCombos = vecCurrentGiftCombos;
            cout << endl << "### New Best: " << bestFoundAvg << endl;
        }
        return;
    }    

    // If no gifts left, there is a trouble, I think

    // Get each Permutation of gifts from the gifts left
    set<vector<int> > v = FindPermutations(vecGiftItems.size(), vecUsedGifts);
   
    // Go through each combo of gifts
    set<vector<int> >:: iterator it1;
    for( it1 = v.begin(); it1!=v.end(); ++it1)
    {
        // Get the gift combo
        vector<int> row = *it1;

        bool bIsGiftValid = true;
        bool bHasMedGift = false;
        bool bHasLrgGift = false;

        // Clear this gift combo from global vector
        vecCurrentGiftCombos[nCurrentChild].clear();

        for (int val: row)
        {
            // Prune by age
            if(!isLogicMatch(nCurrentChild, val))
            {
                bIsGiftValid = false;
                break;
            }
            // Check that we have both sizes
            if(!bHasMedGift &&
                std::binary_search(vecMedGifts.begin(), vecMedGifts.end(), val))
                    bHasMedGift = true;
            if(!bHasLrgGift &&
                std::binary_search(vecLrgGifts.begin(), vecLrgGifts.end(), val))
                    bHasLrgGift = true;

            // Add to the vector of new gifts to use
            vecCurrentGiftCombos[nCurrentChild].push_back(val);
        }
        // If gift combo is not age-valid or child
        // does not both sizes, then not a valid option
        if(!bIsGiftValid || !(bHasMedGift && bHasLrgGift))
            continue;

        std::cout  << "c" << nCurrentChild << ": ";
        for (int val: row)
        {
            std::cout << val << "|";
        }
        std::cout << std::endl;

        // Set the child's global gift combo
        // just in case this is the winner path
        //vecCurrentGiftCombos[nCurrentChild] = vecNewUsedGifts;

        // Add the combo to the Used Gift List
        vector<int> vecNewUsedGifts = vecUsedGifts;

        vecNewUsedGifts.insert(vecNewUsedGifts.end(), 
            vecCurrentGiftCombos[nCurrentChild].begin(), 
            vecCurrentGiftCombos[nCurrentChild].end() );

        // Recurse on this child/gift combo
        processChild(nNewChild, vecNewUsedGifts);
    }
    
}
