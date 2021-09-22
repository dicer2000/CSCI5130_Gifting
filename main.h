/********************************************
 * main.h
 *
 * Brett Huffman
 * CMP SCI 5130 - Gifting Project
 * Due Oct 27, 2021
 * sharedStructures.h file for project
 ********************************************/
#ifndef MAIN_H
#define MAIN_H

#include <string>

using namespace std;

//***************************************************
// Structures
//***************************************************
typedef unsigned int uint;

struct Child {
    string childName;   // Child1, Child2, etc
    int age;            // Age of child
};

struct Gift {
    string giftName;        // G1, G2, etc
    float price;            // Gift price
    float size;             // Gift Size
    bool isAgeDependent;    // Consider childs age?
    int ageMin;             // The min age for this gift
    int ageMax;             // The max age for this gift
};

#endif // MAIN_H

