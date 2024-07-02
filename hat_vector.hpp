#ifndef HAT_VECTOR_HPP
#define HAT_VECTOR_HPP

#include <cstddef>
#include <memory>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <algorithm>

template <typename T, typename Allocator>
class hat_vector;

template <typename T, typename Allocator>
void swap(hat_vector<T, Allocator> &first, hat_vector<T, Allocator> &second);

template <typename T, typename Allocator = std::allocator<T>>
class hat_vector
{
private:
    template <bool Const>
    class iterator_impl;
    
public:
    using value_type = T;
    using allocator_type = Allocator;
    
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    using reference = value_type&;
    using const_reference = const value_type&;
    
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    
    using iterator = iterator_impl<false>;
    using const_iterator = iterator_impl<true>;
    
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    hat_vector();
    ~hat_vector();
    
    hat_vector(const hat_vector<T, Allocator> &other);
    hat_vector<T, Allocator>& operator=(hat_vector<T, Allocator> other);
    
    hat_vector(hat_vector<T, Allocator> &&other);
    
    T& operator[](std::size_t pos);
    const T& operator[](std::size_t pos) const;

    void push_back(const T& element);
    void pop_back();
    
    iterator insert(const_iterator pos, std::size_t count, const T& value);
    iterator insert_opt(const_iterator pos, std::size_t count, const T& value);
    
    iterator erase(const_iterator first, const_iterator last);
    iterator erase(const_iterator pos);
    
    void clear();
    
    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;
    
    void reserve(std::size_t new_capacity);
    
    friend void swap<>(hat_vector<T, Allocator> &first, hat_vector<T, Allocator> &second);
    
    iterator begin();
    iterator end();
    
    const_iterator cbegin();
    const_iterator cend();
    
    reverse_iterator rbegin();
    reverse_iterator rend();
    
    const_reverse_iterator crbegin();
    const_reverse_iterator crend();
    
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

namespace
{
    std::size_t smallest_power_of_two_to_hold(std::size_t n)
    {
        if (n == 0) {
            return 0;
        }
        
        std::size_t power = 0;
        
        --n;
        while (n > 0) {
            n >>= 1;
            ++power;
        }
        
        return power;
    }
}

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
typename hat_vector<T, Allocator>::iterator hat_vector<T, Allocator>::insert(hat_vector<T, Allocator>::const_iterator pos, std::size_t count, const T& value)
{
    const auto pos_idx = pos - cbegin();
    
    if (count == 0) {
        return begin() + pos_idx;
    }
    
    const std::size_t new_size = m_size + count;
    reserve(new_size);
    
    const auto r_bucket = (new_size - 1) >> m_power;
    const auto l_bucket = (pos_idx + count) >> m_power;
    
    const auto bucket_size = 1 << m_power;
    const auto bp = count & ((1 << m_power) - 1);
    const auto spacing = count >> m_power;
    
    for (auto bucket = r_bucket; bucket > l_bucket; --bucket) {
        std::uninitialized_copy_n(m_data[bucket - spacing], bucket_size - bp, m_data[bucket] + bp);
        std::uninitialized_copy_n(m_data[bucket - spacing - 1] + bucket_size - bp, bp, m_data[bucket]);
    }
    std::uninitialized_copy_n(m_data[l_bucket - spacing], bucket_size - bp, m_data[l_bucket] + bp);
    
    for (std::size_t i = pos_idx; i < pos_idx + count; ++i) {
        (*this)[i] = value;
    }
    
    m_size = new_size;
    
    return begin() + pos_idx;
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator hat_vector<T, Allocator>::erase(const_iterator first, const_iterator last)
{
    std::size_t erased = std::distance(first, last);

    iterator nc_first = begin() + (first - cbegin());
    iterator nc_last = begin() + (last - cbegin());

    std::uninitialized_copy(last, cend(), nc_first);
        
    m_size -= erased;
    
    return nc_first;
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::iterator hat_vector<T, Allocator>::erase(const_iterator pos)
{
    return erase(pos, pos + 1);
}

template <typename T, typename Allocator>
void hat_vector<T, Allocator>::clear()
{
    erase(cbegin(), cend());
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
bool hat_vector<T, Allocator>::empty() const
{
    return m_size == 0;
}

template <typename T, typename Allocator>
void hat_vector<T, Allocator>::reserve(std::size_t new_capacity)
{
    if (new_capacity <= m_capacity) {
        return;
    }
    
    std::size_t required_power = smallest_power_of_two_to_hold(new_capacity) >> 1;
    if (required_power > m_power) {
        resize_to_higher(required_power);
    }
        
    while (m_capacity < new_capacity) {
        allocate_bucket();
    }
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
typename hat_vector<T, Allocator>::const_iterator hat_vector<T, Allocator>::cbegin()
{
    return const_iterator(*this, 0);
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::const_iterator hat_vector<T, Allocator>::cend()
{
    return const_iterator(*this, m_size);
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
typename hat_vector<T, Allocator>::const_reverse_iterator hat_vector<T, Allocator>::crbegin()
{
    return hat_vector<T, Allocator>::const_reverse_iterator(cend());
}

template <typename T, typename Allocator>
typename hat_vector<T, Allocator>::const_reverse_iterator hat_vector<T, Allocator>::crend()
{
    return hat_vector<T, Allocator>::const_reverse_iterator(cbegin());
}

template <typename T, typename Allocator>
template <bool Const>
class hat_vector<T, Allocator>::iterator_impl
{
public:
    using iterator_category = std::random_access_iterator_tag;
    
    using difference_type = hat_vector<T, Allocator>::difference_type;
    using value_type = hat_vector<T, Allocator>::value_type;
    using pointer = std::conditional_t<Const, hat_vector<T, Allocator>::const_pointer, hat_vector<T, Allocator>::pointer>;
    using reference = std::conditional_t<Const, hat_vector<T, Allocator>::const_reference, hat_vector<T, Allocator>::reference>;
    
    template <bool C = Const, std::enable_if_t<C, int> = false>
    iterator_impl(const iterator_impl<false> &other)
        : m_internal(other.m_internal), m_pos(other.m_pos)
    {
    }
    
    iterator_impl(hat_vector<T, Allocator> &internal, std::size_t pos)
        : m_internal(&internal), m_pos(pos)
    {
    }
    
    inline reference operator*() const { return (*m_internal)[m_pos]; };
    inline pointer operator->() { return &(*m_internal)[m_pos]; };
    inline reference operator[](difference_type n) { return (*m_internal)[m_pos + n]; };
    
    inline iterator_impl& operator++() { ++m_pos; return *this; };
    inline iterator_impl operator++(int) { iterator_impl copy = *this; ++m_pos; return copy; };
    
    inline iterator_impl& operator--() { --m_pos; return *this; };
    inline iterator_impl operator--(int) { iterator_impl copy = *this; --m_pos; return copy; };
    
    inline iterator_impl& operator +=(difference_type n) { m_pos += n; return *this; };
    inline iterator_impl& operator -=(difference_type n) { m_pos -= n; return *this; };
    
    inline friend iterator_impl operator+(const iterator_impl &left, difference_type n) { return iterator_impl(*left.m_internal, left.m_pos + n); }
    inline friend iterator_impl operator+(difference_type n, const iterator_impl &right) { return iterator_impl(*right.m_internal, right.m_pos + n); }
    
    inline friend iterator_impl operator-(const iterator_impl &left, difference_type n)
    {
        return iterator_impl(*left.m_internal, left.m_pos - n);
    }
    
    inline friend difference_type operator-(const iterator_impl &left, const iterator_impl &right)
    {
        return static_cast<difference_type>(left.m_pos) - static_cast<difference_type>(right.m_pos);
    } 
    
    inline friend bool operator==(const iterator_impl &left, const iterator_impl &right) { return left.m_pos == right.m_pos; }
    inline friend bool operator!=(const iterator_impl &left, const iterator_impl &right) { return left.m_pos != right.m_pos; }
    
    inline friend bool operator<(const iterator_impl &left, const iterator_impl &right) { return left.m_pos < right.m_pos; }
    inline friend bool operator>(const iterator_impl &left, const iterator_impl &right) { return left.m_pos > right.m_pos; }
    
    inline friend bool operator<=(const iterator_impl &left, const iterator_impl &right) { return left.m_pos <= right.m_pos; }
    inline friend bool operator>=(const iterator_impl &left, const iterator_impl &right) { return left.m_pos >= right.m_pos; }
    
private:
    friend iterator_impl<true>;

    hat_vector<T, Allocator> *m_internal;
    std::size_t m_pos;
};

#endif // HAT_VECTOR_HPP
