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

// FindPermutations - A function to find all permutations
// of a given number of elements
// n = Max Number of elements
// exclusionSet = The elements to disregard from the end list
// returns an vector of ints
std::vector<std::string> FindPermutations(int n, std::vector<int> &exclusionSet)
{
    // Fill this vector with all the permutations
    std::vector<std::string> permutations;
    int k = n - 1;

    for(int j = 1; j < k + 1; j++)
    {
        std::vector<bool> v(n);
        std::fill(v.begin(), v.begin() + j, true);

        // Loop through our vector mask determining
        // all combinations of k elements
        do {
            bool bUseSet = true;
            std::string strCombo = "";
            for (int i = 0; i < n; ++i) {

                // By saying i!=X, removes X from the possible combinations
                if (v[i]) {
                    if(!std::binary_search(exclusionSet.begin(), exclusionSet.end(), i))
                        strCombo += std::to_string(i) + " ";
                    else
                        bUseSet = false;
                }
            }
            // Keep the the combo if it doesn't have a excluded element in it
            if(bUseSet)
                permutations.push_back(strCombo.substr(0, strCombo.size() - 1));

        } while (std::prev_permutation(v.begin(), v.end()));

    }
    return permutations;
}