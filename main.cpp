#include "hat_vector.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    for (int i = 0; i < 10; i++) {
        v.push_back(i);
    }
    
    v.insert(v.cbegin() + 8, 9, 42);
    
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    
    v.erase(v.cbegin() + 2, v.cend() - 1);
    v.erase(v.cbegin() + 1);
    
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    return 0;
}
