#include "hat_vector.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    for (int i = 0; i < 1'000'000; i++)
        v.push_back(i);
        
    std::sort(v.begin(), v.end(), std::greater<>{});
    
    for (auto x : v) {
        std::cout << x << std::endl;
    }
    
    /*
    int sum = 0;
    for (auto it = v.rbegin(); it != v.rend(); it++) {
        sum += *it;
    }
        
    std::cout << sum << std::endl;
    */

    return 0;
}
