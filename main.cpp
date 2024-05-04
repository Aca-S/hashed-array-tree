#include "hat_vector.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
        
    v.reserve(1000000);
    for (int i = 0; i < 1000000; i++) {
        v.push_back(i);
    }
    
    std::sort(v.begin(), v.end());

    return 0;
}
