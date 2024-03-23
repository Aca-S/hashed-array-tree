#include "hat_vector.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    for (int i = 0; i < 1'000; i++)
        v.push_back(i);
        
    auto it = v.begin();
    *it = 4;
        
    std::sort(v.begin(), v.end(), std::greater<>{});
    
    for (auto it = v.crbegin(); it != v.crend(); it++)
        std::cout << *it << std::endl;

    return 0;
}
