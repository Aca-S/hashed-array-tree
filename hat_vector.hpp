#ifndef HAT_VECTOR_HPP
#define HAT_VECTOR_HPP

#include <cstddef>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class hat_vector
{
public:
    T& operator[](std::size_t pos);
    const T& operator[](std::size_t pos) const;

    void push_back(const T& element);
    
private:
    Allocator m_allocator;
};

template <typename T, typename Allocator>
void hat_vector<T, Allocator>::push_back(const T& element)
{
}

#endif // HAT_VECTOR_HPP
