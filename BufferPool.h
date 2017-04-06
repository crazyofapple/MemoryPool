#pragma once
#include <cassert>
#include <cstddef>
#include <forward_list>


class BufferPool;
class Link final{
public:
	void initialize() {
		Next = nullptr;
	}
	bool empty() const {
		return Next == nullptr;
	}
	Link *pop() {
		Link *top = Next;
		if (Next != nullptr) {
			Next = Next->Next;
		}
		return top;
	}
	void push(Link *newLink) {
		assert(newLink != nullptr && "Invalid link");
		newLink->Next = Next;
		Next = newLink;
	}
private:
	Link *Next = nullptr;
};

class Chunk final {
	friend BufferPool;
private:
	std::size_t NumberOfEmptySlots = 0;
	Link EmptySlotsList;
public:
};

class ChunkHeader {
	friend BufferPool;
private:
	std::size_t Index = 0;
public:
};

class BufferPool {
public:
	BufferPool(std::size_t ObjectSize, std::size_t ObjectAlignment, std::size_t chunkSize);
	virtual ~BufferPool(){
		destory();
	}
	void allocateChunk();
	void *allocateMemory();
	void deallocateMemory(void *address);
	void destory();
	std::size_t getChunkSize(void) const{
		return ChunkAlignment + sizeof(Chunk) + NumberOfObjectsPerChunk * (ObjectSize + ObjectAlignment + sizeof(ChunkHeader));
	}
private:
	std::forward_list<Chunk* > Chunks;
	volatile std::size_t NumberOfObjectsPerChunk = 0;
	std::size_t ObjectSize = sizeof(Link);
	std::size_t NumberOfChunks = 0;
	std::size_t NumberOfFreeObjects = 0;
	std::size_t	NumberOfObjects = 0;
	std::size_t ChunkAlignment = 0;
	std::size_t ObjectAlignment = 0;
};
