#ifndef HAT_VECTOR_HPP
#define HAT_VECTOR_HPP

#include <cstddef>

template <typename T>
class hat_vector
{
public:
    T& operator[](std::size_t pos);
    const T& operator[](std::size_t pos) const;

    void push_back(const T& element);
};

template <typename T>
void hat_vector<T>::push_back(const T& element)
{
}

#endif // HAT_VECTOR_HPP
