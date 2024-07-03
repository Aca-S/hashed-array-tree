#include "hat_vector.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    for (int i = 1; i <= 10; i++) {
        v.push_back(i);
    }
    
    v.erase(v.begin() + 2, v.begin() + 5);
    
    for (auto el : v) {
        std::cout << el << " ";
    }
    std::cout << std::endl;

    return 0;
}
