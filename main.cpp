#include "hat_vector.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
        
    v.reserve(128);
    for (int i = 0; i < 128; i++) {
        v.push_back(7);
    }
    
    std::cout << v.size() << std::endl;
    
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    return 0;
}
