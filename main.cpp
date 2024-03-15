#include "hat_vector.hpp"

#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    for (int i = 0; i < 1'000'000; i++)
        v.push_back(i);
        
    int sum = 0;
    for (int i = 0; i < v.size(); i++)
        sum += v[i];
        
    std::cout << sum << std::endl;

    return 0;
}
