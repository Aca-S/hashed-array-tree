#ifndef HAT_VECTOR_HPP
#define HAT_VECTOR_HPP

#include <cstddef>
#include <memory>
#include <cassert>
#include <iterator>

template <typename T, typename Allocator>
class hat_vector;

template <typename T, typename Allocator>
void swap(hat_vector<T, Allocator> &first, hat_vector<T, Allocator> &second);

template <typename T, typename Allocator = std::allocator<T>>
class hat_vector
{
public:
    hat_vector();
    ~hat_vector();
    
    hat_vector(const hat_vector<T, Allocator> &other);
    hat_vector<T, Allocator>& operator=(hat_vector<T, Allocator> other);
    
    hat_vector(hat_vector<T, Allocator> &&other);
    
    T& operator[](std::size_t pos);
    const T& operator[](std::size_t pos) const;

    void push_back(const T& element);
    void pop_back();
    
    std::size_t size() const;
    std::size_t capacity() const;
    
    friend void swap<>(hat_vector<T, Allocator> &first, hat_vector<T, Allocator> &second);
    
    class iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    
    iterator begin();
    iterator end();
    
    reverse_iterator rbegin();
    reverse_iterator rend();
    
private:
    void resize_to_higher(std::size_t new_power);
    void deallocate();
    void allocate_bucket();

    Allocator m_allocator;
    T** m_data;
    std::size_t m_power = 1;
    std::size_t m_capacity = 0;
    std::size_t m_size = 0;
};

template <typename T, typename Allocator>
void swap(hat_vector<T, Allocator> &first, hat_vector<T, Allocator> &second)
{
    std::swap(first.m_allocator, second.m_allocator);
    std::swap(first.m_data, second.m_data);
    std::swap(first.m_power, second.m_power);
    std::swap(first.m_capacity, second.m_capacity);
    std::swap(first.m_size, second.m_size);
}

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
hat_vector<T, Allocator>::hat_vector(const hat_vector<T, Allocator> &other)
    : m_allocator(other.m_allocator), m_power(other.m_power), m_capacity(other.m_capacity), m_size(other.m_size)
{
    const std::size_t bucket_size = 1 << m_power;
    const std::size_t buckets_to_alloc = m_capacity == 0 ? 0 : 1 + (m_capacity - 1 >> m_power);
    
    m_data = new T*[bucket_size];
    for (std::size_t i = 0; i < buckets_to_alloc; ++i) {
        m_data[i] = m_allocator.allocate(bucket_size);
        std::uninitialized_copy_n(other.m_data[i], bucket_size, m_data[i]);
    }
}

template <typename T, typename Allocator>
hat_vector<T, Allocator>& hat_vector<T, Allocator>::operator=(hat_vector<T, Allocator> other)
{
    swap(*this, other);
    return *this;
}

template <typename T, typename Allocator>
hat_vector<T, Allocator>::hat_vector(hat_vector<T, Allocator> &&other)
    : hat_vector<T, Allocator>()
{
    swap(*this, other);
}

template <typename T, typename Allocator>
void hat_vector<T, Allocator>::resize_to_higher(std::size_t new_power)
{
    assert(new_power > m_power && "hat_vector<T, Allocator>::allocate_higher requires a power higher than hat_vector<T, Allocator>::m_power");
    
    const std::size_t old_bucket_size = 1 << m_power;
    const std::size_t new_bucket_size = 1 << new_power;
    
    const std::size_t buckets_to_alloc = m_size == 0 ? 0 : 1 + (m_size - 1 >> new_power);
    
    T** new_data = new T*[new_bucket_size];
    for (std::size_t i = 0; i < buckets_to_alloc; ++i) {
        new_data[i] = m_allocator.allocate(new_bucket_size);
    }
    
    for (std::size_t copied = 0; copied < m_size; copied += old_bucket_size) {
        std::uninitialized_copy_n(m_data[copied >> m_power], old_bucket_size, new_data[copied >> new_power] + (copied & (new_bucket_size - 1)));
    }
    
    deallocate();
    
    m_data = new_data;
    m_power = new_power;
    m_capacity = buckets_to_alloc << new_power;
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
        resize_to_higher(m_power + 1);
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
void hat_vector<T, Allocator>::pop_back()
{
    --m_size;
    m_data[m_size >> m_power][m_size & ((1 << m_power) - 1)].~T();
}

template <typename T, typename Allocator>
std::size_t hat_vector<T, Allocator>::size() const
{
    return m_size;
}

template <typename T, typename Allocator>
std::size_t hat_vector<T, Allocator>::capacity() const
{
    return m_capacity;
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

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator hat_vector<T, Allocator>::begin()
{
    return iterator(*this, 0);
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator hat_vector<T, Allocator>::end()
{
    return iterator(*this, m_size);
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::reverse_iterator hat_vector<T, Allocator>::rbegin()
{
    return hat_vector<T, Allocator>::reverse_iterator(end());
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::reverse_iterator hat_vector<T, Allocator>::rend()
{
    return hat_vector<T, Allocator>::reverse_iterator(begin());
}

template <typename T, typename Allocator>
class hat_vector<T, Allocator>::iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    
    iterator(hat_vector<T, Allocator> &internal, std::size_t pos);
    
    reference operator*() const;
    pointer operator->();
    reference operator[](difference_type n);
    
    iterator& operator++();
    iterator operator++(int);
    
    iterator& operator--();
    iterator operator--(int);
    
    iterator& operator +=(difference_type n);
    iterator& operator -=(difference_type n);
    
    friend iterator operator+(const iterator &left, difference_type n) { return iterator(*left.m_internal, left.m_pos + n); }
    friend iterator operator+(difference_type n, const iterator &right) { return iterator(*right.m_internal, right.m_pos + n); }
    
    friend iterator operator-(const iterator &left, difference_type n) { return iterator(*left.m_internal, left.m_pos - n); }
    friend difference_type operator-(const iterator &left, const iterator &right) { return static_cast<difference_type>(left.m_pos) - static_cast<difference_type>(right.m_pos); } 
    
    friend bool operator==(const iterator &left, const iterator &right) { return left.m_pos == right.m_pos; }
    friend bool operator!=(const iterator &left, const iterator &right) { return left.m_pos != right.m_pos; }
    
    friend bool operator<(const iterator &left, const iterator &right) { return left.m_pos < right.m_pos; }
    friend bool operator>(const iterator &left, const iterator &right) { return left.m_pos > right.m_pos; }
    
    friend bool operator<=(const iterator &left, const iterator &right) { return left.m_pos <= right.m_pos; }
    friend bool operator>=(const iterator &left, const iterator &right) { return left.m_pos >= right.m_pos; }
    
private:
    hat_vector<T, Allocator> *m_internal;
    std::size_t m_pos;
};

template <typename T, typename Allocator>
hat_vector<T, Allocator>::iterator::iterator(hat_vector<T, Allocator> &internal, std::size_t pos)
    : m_internal(&internal), m_pos(pos)
{
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator::reference hat_vector<T, Allocator>::iterator::operator*() const
{
    return (*m_internal)[m_pos];
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator::pointer hat_vector<T, Allocator>::iterator::operator->()
{
    return &(*m_internal)[m_pos];
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator::reference hat_vector<T, Allocator>::iterator::operator[](difference_type n)
{
    return (*m_internal)[m_pos + n];
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator& hat_vector<T, Allocator>::iterator::operator++()
{
    ++m_pos;
    return *this;
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator hat_vector<T, Allocator>::iterator::operator++(int)
{
    iterator copy = *this;
    ++m_pos;
    return copy;
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator& hat_vector<T, Allocator>::iterator::operator--()
{
    --m_pos;
    return *this;
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator hat_vector<T, Allocator>::iterator::operator--(int)
{
    iterator copy = *this;
    --m_pos;
    return copy;
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator& hat_vector<T, Allocator>::iterator::operator+=(difference_type n)
{
    m_pos += n;
    return *this;
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator& hat_vector<T, Allocator>::iterator::operator-=(difference_type n)
{
    m_pos -= n;
    return *this;
}

#endif // HAT_VECTOR_HPP
