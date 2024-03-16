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

    return 0;
}
