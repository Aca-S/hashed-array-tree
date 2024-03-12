#include "hat_vector.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    hat_vector<int> v;
    
    for (int i = 0; i < 10000; i++)
        v.push_back(i);
        
    for (int i = 0; i < v.size(); i++)
        std::cout << v[i] << std::endl;

    return 0;
}
