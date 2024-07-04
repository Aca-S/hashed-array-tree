#include "../hat_vector.hpp"
#include <vector>
#include <iostream>

int main(int argc, char *argv[])
{
    std::vector<int> v;

    for (int i = 0; i < 1024; ++i) {
        v.push_back(i);
        std::cout << v.capacity() - v.size() << ",";
    }
    std::cout << std::endl;

    return 0;
}
