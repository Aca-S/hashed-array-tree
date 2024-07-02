#include "hat_vector.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    v.reserve(10000);
    for (int i = 1; i <= 10000; i++) {
        v.insert(v.begin(), 1, i);
    }
    
    std::sort(v.begin(), v.end());
    
    for (auto el : v) {
        std::cout << el << " ";
    }
    std::cout << std::endl;

    return 0;
}
