#pragma once
#include <atomic>

namespace bench {

template <typename T>
void AddRef(T* value);

template <typename T>
void RemoveRef(T* value);

template <typename T>
class RefHandle {
public:
	T* data;

	RefHandle() {
		this->data = nullptr;
	}

	RefHandle(T* data) {
		this->data = data;
		AddRef(this->data);
	}

	RefHandle(RefHandle&& other) {
		this->data = other.data;
		other.data = nullptr;
	}

	RefHandle(const RefHandle& other) {
		this->data = other.data;
		AddRef(this->data);
	}

	RefHandle& operator=(RefHandle&& other) {
		this->data = other.data;
		other.data = nullptr;
		return *this;
	}

	RefHandle& operator=(const RefHandle& other) {
		this->data = other.data;
		AddRef(this->data);
		return *this;
	}

	~RefHandle() {
		if (this->data) {
			RemoveRef(this->data);
			this->data = nullptr;
		}
	}

	operator T*() const {
		return this->data;
	}
};

}