#ifndef HAT_VECTOR_HPP
#define HAT_VECTOR_HPP

#include <cstddef>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class hat_vector
{
public:
    hat_vector();
    ~hat_vector();
    
    T& operator[](std::size_t pos);
    const T& operator[](std::size_t pos) const;

    void push_back(const T& element);
    
private:
    void resize();
    void deallocate();
    void allocate_bucket();

    Allocator m_allocator;
    T** m_data;
    std::size_t m_power = 1;
    std::size_t m_capacity = 0;
    std::size_t m_size = 0;
};

template <typename T, typename Allocator>
hat_vector<T, Allocator>::hat_vector()
{
    m_data = new T*[1 << m_power];
}

template <typename T, typename Allocator>
hat_vector<T, Allocator>::~hat_vector()
{
    deallocate();
}

template <typename T, typename Allocator>
void hat_vector<T, Allocator>::resize()
{
    const std::size_t old_bucket_size = 1 << m_power;
    const std::size_t new_bucket_size = old_bucket_size << 1;
    const std::size_t buckets_to_fill = old_bucket_size >> 1;
    
    T** new_data = new T*[new_bucket_size];
    for (std::size_t i = 0; i < buckets_to_fill; ++i) {
        new_data[i] = m_allocator.allocate(new_bucket_size);
        std::uninitialized_copy_n(m_data[i << 1], old_bucket_size, new_data[i]);
        std::uninitialized_copy_n(m_data[(i << 1) + 1], old_bucket_size, new_data[i] + old_bucket_size);
    }
    
    deallocate();
    m_data = new_data;
    ++m_power;
}

template <typename T, typename Allocator>
void hat_vector<T, Allocator>::deallocate()
{
    const std::size_t num_of_buckets = m_capacity >> m_power;
    const std::size_t bucket_size = 1 << m_power;
    for (std::size_t i = 0; i < num_of_buckets; ++i) {
        m_allocator.deallocate(m_data[i], bucket_size);
    }
    delete[] m_data;
}

template <typename T, typename Allocator>
void hat_vector<T, Allocator>::allocate_bucket()
{
    const std::size_t bucket_size = 1 << m_power;
    m_data[m_capacity >> m_power] = m_allocator.allocate(bucket_size);
    m_capacity += bucket_size;
}

template <typename T, typename Allocator>
void hat_vector<T, Allocator>::push_back(const T& element)
{
    // Resize if needed
    if (m_size == 1 << m_power << m_power) {
        resize();
    }
    
    // Allocate another bucket if needed
    if (m_size == m_capacity) {
        allocate_bucket();
    }
    
    // Set the element
    m_data[m_size >> m_power][m_size & ((1 << m_power) - 1)] = element;
    ++m_size;
}

template <typename T, typename Allocator>
T& hat_vector<T, Allocator>::operator[](std::size_t pos)
{
    return m_data[pos >> m_power][pos & ((1 << m_power) - 1)];
}

template <typename T, typename Allocator>
const T& hat_vector<T, Allocator>::operator[](std::size_t pos) const
{
    return m_data[pos >> m_power][pos & ((1 << m_power) - 1)];
}

#endif // HAT_VECTOR_HPP
