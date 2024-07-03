#include "hat_vector.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    for (int i = 1; i <= 17; i++) {
        v.push_back(i);
    }

    v.pop_back();
    v.shrink_to_fit();

    v.erase(v.begin(), v.begin() + 4);
    v.shrink_to_fit();
    
    for (auto el : v) {
        std::cout << el << " ";
    }
    std::cout << std::endl;

    return 0;
}
