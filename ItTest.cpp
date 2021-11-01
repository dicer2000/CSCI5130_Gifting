#include <iostream>
#include <algorithm>
#include <vector>

int main() {
   int n, r;
   std::cin >> n;
   std::cin >> r;

   std::vector<bool> v(n);
   std::fill(v.begin(), v.begin() + r, true);

   do {
       for (int i = 0; i < n; ++i) {

           // By saying i!=X, removes X from the possible combinations
           if (v[i] && i!=2) {
               std::cout << i << " ";
           }
       }
       std::cout << "\n";
   } while (std::prev_permutation(v.begin(), v.end()));
   return 0;
}