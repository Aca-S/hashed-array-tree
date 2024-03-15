#include "hat_vector.hpp"

#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    for (int i = 0; i < 1'000'000; i++)
        v.push_back(i);
        
    hat_vector<int> v2(v);
    hat_vector<int> v3;
    v3 = v2;
    
    swap(v3, v);
    
    for (int i = 0; i < 1'000'000; i++)
        v3.pop_back();
    
    int sum = 0;
    for (int i = 0; i < v3.size(); i++)
        sum += v3[i];
        
    std::cout << sum << std::endl;

    return 0;
}
