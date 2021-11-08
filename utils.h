/********************************************
 * Gifting Project - Utility file
 * Brett Huffman
 * CMP SCI 5130 - Project 1
 * Due Nov 19, 2021
 * Utility functions for processing
 ********************************************/


#include <algorithm>
#include <vector>
#include <string>

// FindPermutations - A function to find all unique permutations
// of a given number of elements.  The set keeps it unique
// n = Max Number of elements
// exclusionSet = The elements to disregard from the end list
// returns an set of vector of ints
std::set<std::vector<int> > FindPermutations(const int n, const std::vector<int> &exclusionSet)
{
    // Fill this 2D vector with all the permutations
    std::set<std::vector<int> > permutations;
    int k = n - 1;

    for(int j = 1; j < k + 1; j++)
    {
        std::vector<bool> v(n);
        std::fill(v.begin(), v.begin() + j, true);

        // Loop through our vector mask determining
        // all combinations of k elements
        do {
            std::vector<int> vCombo;
            for (int i = 0; i < n; ++i) {

                // By saying i!=X, removes X from the possible combinations
                if (v[i] && !std::binary_search(exclusionSet.begin(), exclusionSet.end(), i))
                    vCombo.push_back(i);
            }
            // Keep the the combo if it doesn't have a excluded element in it
            if(vCombo.size() > 0)
                permutations.insert(vCombo);

        } while (std::prev_permutation(v.begin(), v.end()));

    }
    return permutations;
}