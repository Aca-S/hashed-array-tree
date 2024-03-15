#include "hat_vector.hpp"

#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    for (int i = 0; i < 1'000'000; i++)
        v.push_back(i);
        
    hat_vector<int> v2(v);
        
    int sum = 0;
    for (int i = 0; i < v2.size(); i++)
        sum += v2[i];
        
    std::cout << sum << std::endl;

    return 0;
}
