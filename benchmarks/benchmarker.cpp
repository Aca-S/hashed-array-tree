#include "../hat_vector.hpp"
#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include <functional>

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

void benchmarker(const std::vector<std::size_t> &sizes, std::size_t repeats, const std::function<void(std::size_t)> &func)
{
    for (const auto n : sizes) {
        double elapsed_time = 0;
        for (std::size_t repeat = 0; repeat < repeats; ++repeat) {
            const auto start = std::chrono::steady_clock::now();
            func(n);
            const auto end = std::chrono::steady_clock::now();

            const std::chrono::duration<double> elapsed = end - start;
            elapsed_time += elapsed.count();
        }
        elapsed_time /= repeats;

        std::cout << "n = " << n << ", time = " << elapsed_time << "s" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    benchmarker({1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000}, 5, &push_back_and_sort);

    return 0;
}
