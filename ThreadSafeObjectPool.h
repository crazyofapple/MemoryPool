#pragma once
#include "BufferPool.h"
#include "SpinLock.h"
#include <mutex>

template<typename T, std::size_t Alignment = 16, std::size_t NumberOfObjectPerChunk = 1024>
class ThreadSafeObjectPool : public BufferPool {
public:
	ThreadSafeObjectPool() : BufferPool(sizeof(T), Alignment, NumberOfObjectPerChunk) {}

	template<typename... Args>
	T* create(Args&&... args) {
		void* address = nullptr;
		{
			std::lock_guard<decltype(spinLock)> lock(spinLock);
			address = allocateMemory();
		}
		return new (address) T(std::forward<Args>(args)...);
	}

	void destroy(void * address) {
		destory(reinterpret_cast<T*>(address));
	}
	void destory(T* object) {
		assert(object && "Invalid object");
		object->~T();
		std::lock_guard<decltype(spinLock)> lock(spinLock);
		deallocateMemory(object);
	}
private:
	SpinLock spinLock;

};