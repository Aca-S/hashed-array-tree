#include "../hat_vector.hpp"
#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <random>
#include <functional>
#include <cstdlib>

void push_back_and_sort(std::size_t n)
{
    std::uniform_int_distribution<> dist(1, 10);
    std::mt19937 gen(1234);

    std::vector<int> v;

    for (std::size_t i = 0; i < n; ++i) {
        v.push_back(dist(gen));
    }

    std::sort(v.begin(), v.end());
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Missing test size argument" << std::endl;
        return 1;
    }

    const auto n = std::atoi(argv[1]);

    push_back_and_sort(n);

    return 0;
}
