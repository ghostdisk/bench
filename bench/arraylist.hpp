#pragma once
#include <bench/core/common.hpp>
#include <initializer_list>

namespace bench {

/**
 * ArrayList<T> is a simple std::vector replacement. The advantages are:
 * 1. Better integration with other bench types (slices, arenas)
 * 2. More lightweight, no insane overloads
 * 2. Better compile times - <vector> includes add ~100ms on MSVC
 */
template <typename T>
class ArrayList {
public:
	T* m_data = nullptr;
	U32 m_size = 0;
	U32 m_capacity = 0;

	ArrayList() {
	}

	ArrayList(const T* values, U32 size) {
		CopyFrom(values, size);
	}

	ArrayList(std::initializer_list<T> list)
		: ArrayList(list.begin(), list.size()) {
	}

	ArrayList(ArrayList&& other) {
		MoveFrom(Move(other));
	}

	ArrayList(const ArrayList& other)
		: ArrayList(other.m_data, other.m_size) {
	}

	void operator=(const ArrayList& other) {
		assert(this != &other);
		Destroy();
		CopyFrom(other.m_data, other.m_size);
	}

	void operator=(ArrayList&& other) {
		assert(this != &other);
		Destroy();
		MoveFrom(Move(other));
	}

	~ArrayList() {
		Destroy();
	}

	inline T* begin() {
		return m_data;
	}

	inline T* end() {
		return m_data + m_size;
	}

	inline const T* begin() const {
		return m_data;
	}

	inline const T* end() const {
		return m_data + m_size;
	}

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


	T& Push(const T& value) {
		if (m_size == m_capacity)
			this->SetCapacity(m_size > 0 ? m_size * 2 : 5);

		new (m_data + m_size) T(value);

		m_size++;
		return m_data[m_size - 1];
	}

	T& Push(T&& value) {
		if (m_size == m_capacity)
			this->SetCapacity(m_size > 0 ? m_size * 2 : 5);

		new (m_data + m_size) T(Move(value));

		m_size++;
		return m_data[m_size - 1];
	}

	template <typename ...Args>
	T& Emplace(Args&& ...args) {
		if (m_size == m_capacity)
			this->SetCapacity(m_size > 0 ? m_size * 2 : 5);

		new (m_data + m_size) T(static_cast<Args&&>(args)...);
		m_size++;
		return m_data[m_size - 1];
	}

	T Pop() {
		assert(m_size > 0);
		T value = Move(m_data[m_size - 1]);
		m_size--;
		return value;
	}

	T& First() {
		return m_data[0];
	}

	T& Last() {
		return m_data[m_size - 1];
	}

	const T& First() const {
		return m_data[0];
	}

	const T& Last() const {
		return m_data[m_size - 1];
	}


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

	void RemoveAtUnsorted(U32 index) {
		assert(m_size);
		if (index < m_size) {
			m_data[index] = Move(m_data[m_size - 1]);
			m_size--;
		}
	}

private:
	void MoveFrom(ArrayList&& other) {
		m_data = other.m_data;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}

	void CopyFrom(const T* values, U32 size) {
		m_data = (T*)malloc(size * sizeof(T));
		m_size = size;
		m_capacity = size;

		for (U32 i = 0; i < size; i++)
			new (m_data + i) T(values[i]);
	}

};

}