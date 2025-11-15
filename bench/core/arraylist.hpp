#pragma once
#include <bench/core/common.hpp>
#include <initializer_list>

namespace bench {

/// std::vector-like dynamic array list.
template <typename T>
class ArrayList {
public:
    /// Pointer to the allocated storage.
    T* m_data = nullptr;

    /// Current number of elements in the array.
    U32 m_size = 0;

    /// Current allocated capacity of the array.
    U32 m_capacity = 0;

    /// Create an empty ArrayList.
    ArrayList() {
    }

    /// Create an ArrayList from an existing range of values.
    /// @param values Pointer to the source array.
    /// @param size Number of elements to copy.
    ArrayList(const T* values, U32 size) {
        CopyFrom(values, size);
    }

    /// Create an ArrayList from an initializer list.
    /// @param list Initializer list of values to copy.
    ArrayList(std::initializer_list<T> list)
        : ArrayList(list.begin(), list.size()) {
    }

    /// Move constructor. Transfers ownership from another ArrayList.
    ArrayList(ArrayList&& other) {
        MoveFrom(Move(other));
    }

    /// Copy constructor. Deep copies the elements from another ArrayList.
    ArrayList(const ArrayList& other)
        : ArrayList(other.m_data, other.m_size) {
    }

    /// Copy assignment operator. Performs deep copy.
    void operator=(const ArrayList& other) {
        assert(this != &other);
        Destroy();
        CopyFrom(other.m_data, other.m_size);
    }

    /// Move assignment operator. Transfers ownership from another ArrayList.
    void operator=(ArrayList&& other) {
        assert(this != &other);
        Destroy();
        MoveFrom(Move(other));
    }

    /// Destroy the ArrayList and free all memory.
    ~ArrayList() {
        Destroy();
    }

    /// Returns an iterator to the beginning of the array.
    inline T* begin() {
        return m_data;
    }

    /// Returns an iterator to the end of the array.
    inline T* end() {
        return m_data + m_size;
    }

    /// Returns a const iterator to the beginning of the array.
    inline const T* begin() const {
        return m_data;
    }

    /// Returns a const iterator to the end of the array.
    inline const T* end() const {
        return m_data + m_size;
    }

    /// Resize the array to a new capacity. Existing elements are moved.
    /// @param new_capacity New allocated capacity.
    void SetCapacity(U32 new_capacity) {
        U32 new_size = new_capacity < m_size ? new_capacity : m_size;
        T* new_data = (T*)malloc(new_capacity * sizeof(T));

        for (U32 i = 0; i < m_size; i++)
            new_data[i] = Move(m_data[i]);

        if (m_data) {
            for (U32 i = new_size; i < m_size; i++)
                m_data[i].~T();
            free(m_data);
        }

        m_data = new_data;
        m_size = new_size;
        m_capacity = new_capacity;
    }

    /// Append a copy of a value to the array.
    /// @param value Value to copy into the array.
    /// @return Reference to the newly added element.
    T& Push(const T& value) {
        if (m_size == m_capacity)
            this->SetCapacity(m_size > 0 ? m_size * 2 : 5);

        new (m_data + m_size) T(value);
        m_size++;
        return m_data[m_size - 1];
    }

    /// Append a value to the array by moving it.
    /// @param value Value to move into the array.
    /// @return Reference to the newly added element.
    T& Push(T&& value) {
        if (m_size == m_capacity)
            this->SetCapacity(m_size > 0 ? m_size * 2 : 5);

        new (m_data + m_size) T(Move(value));
        m_size++;
        return m_data[m_size - 1];
    }

    /// Construct a new element in-place at the end of the array.
    /// @tparam Args Types of constructor arguments.
    /// @param args Arguments to forward to the constructor of T.
    /// @return Reference to the newly emplaced element.
    template <typename ...Args>
    T& Emplace(Args&& ...args) {
        if (m_size == m_capacity)
            this->SetCapacity(m_size > 0 ? m_size * 2 : 5);

        new (m_data + m_size) T(static_cast<Args&&>(args)...);
        m_size++;
        return m_data[m_size - 1];
    }

    /// Remove the last element from the array and return it.
    /// @return The last element (moved out).
    T Pop() {
        assert(m_size > 0);
        T value = Move(m_data[m_size - 1]);
        m_size--;
        return value;
    }

    /// Get a reference to the first element.
    T& First() {
        return m_data[0];
    }

    /// Get a reference to the last element.
    T& Last() {
        return m_data[m_size - 1];
    }

    /// Get a const reference to the first element.
    const T& First() const {
        return m_data[0];
    }

    /// Get a const reference to the last element.
    const T& Last() const {
        return m_data[m_size - 1];
    }

    /// Destroy all elements and free memory.
    void Destroy() {
        if (m_data) {
            for (U32 i = 0; i < m_size; i++)
                m_data[i].~T();
            free(m_data);
        }
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    /// Remove an element by index without preserving order.
    /// @param index Index of the element to remove.
    void RemoveAtUnsorted(U32 index) {
        assert(m_size);
        if (index < m_size) {
            m_data[index] = Move(m_data[m_size - 1]);
            m_size--;
        }
    }

private:
    /// Move internal data from another ArrayList.
    void MoveFrom(ArrayList&& other) {
        m_data = other.m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    /// Deep copy elements from a raw array.
    /// @param values Source values to copy.
    /// @param size Number of elements to copy.
    void CopyFrom(const T* values, U32 size) {
        m_data = (T*)malloc(size * sizeof(T));
        m_size = size;
        m_capacity = size;

        for (U32 i = 0; i < size; i++)
            new (m_data + i) T(values[i]);
    }
};

} // namespace bench
