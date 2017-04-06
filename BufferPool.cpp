#include "BufferPool.h"
using namespace std;

BufferPool::BufferPool(std::size_t objectSize, std::size_t objectAlignment, std::size_t chunkSize)
	: NumberOfObjectsPerChunk(chunkSize) {
	if (objectSize > this->ObjectSize){
		this->ObjectSize = objectSize;
	}
	this->ObjectAlignment = objectAlignment - ((sizeof(ChunkHeader) + this->ObjectSize) % objectAlignment);
	if (this->ObjectAlignment == objectAlignment){
		this->ObjectAlignment = 0;
	}
	ChunkAlignment = objectAlignment - (sizeof(Chunk) % objectAlignment);
	if (this->ChunkAlignment == objectAlignment){
		ChunkAlignment = 0;
	}
}

void BufferPool::destory() {
	for (Chunk const *chunk : Chunks) {
		delete[] reinterpret_cast<unsigned char const *>(chunk);
	}
	Chunks.clear();
}
/// 申请一个新的块 内存布局为 chunk + chunkalignment + numofobjectperchunk个Object (chunkHeader  + object + objectAlignment)
void BufferPool::allocateChunk() {
	const std::size_t chunkSize = getChunkSize();
	Chunk *newChunk = reinterpret_cast<Chunk *>(new unsigned char[chunkSize]);
	newChunk->NumberOfEmptySlots = NumberOfObjectsPerChunk;
	newChunk->EmptySlotsList.initialize();
	Chunks.push_front(newChunk);
	NumberOfFreeObjects += NumberOfObjectsPerChunk;
	++NumberOfChunks;
	unsigned char *data = reinterpret_cast<unsigned char *>(newChunk + 1);
	data += ChunkAlignment;
	for (std::size_t index = 0; index < NumberOfObjectsPerChunk; ++index) {
		reinterpret_cast<ChunkHeader *>(data)->Index = index;
		data += sizeof(ChunkHeader);
		newChunk->EmptySlotsList.push(reinterpret_cast<Link *>(data));
		data += ObjectSize + ObjectAlignment;
	}
}
// 申请一个object的空间
void * BufferPool::allocateMemory() {
	if (NumberOfFreeObjects == 0) {
		allocateChunk();
	}
	for (Chunk *chunk : Chunks) {
		if (chunk->NumberOfEmptySlots != 0) {
			Link *address = chunk->EmptySlotsList.pop();
			--chunk->NumberOfEmptySlots;
			--NumberOfFreeObjects;
			++NumberOfObjects;
			return static_cast<void *>(address);
		}
	}
	assert(!"Never reached");
	return nullptr;
}

void BufferPool::deallocateMemory(void * address) {
	assert(address != nullptr && "Invalid address");
	unsigned char *data = static_cast<unsigned char *>(address) - sizeof(ChunkHeader);
	Chunk *chunk = reinterpret_cast<Chunk *>(data -
		(reinterpret_cast<ChunkHeader *>(data)->Index * (sizeof(ChunkHeader) + ObjectSize + ObjectAlignment)) - ChunkAlignment) - 1;
	++chunk->NumberOfEmptySlots;
	chunk->EmptySlotsList.push(reinterpret_cast<Link *>(address));
	++NumberOfFreeObjects;
	--NumberOfObjects;
	// 每次将回收的chunk放在最前边 为了尽量减少系统内存分配
	if (chunk != Chunks.front()) {
		Chunks.remove(chunk);
		Chunks.push_front(chunk);
	}
	// 回收chunk
	if (chunk->NumberOfEmptySlots == NumberOfObjectsPerChunk) {
		delete[] reinterpret_cast<unsigned char const *>(chunk);
		Chunks.pop_front();
		NumberOfFreeObjects -= NumberOfObjectsPerChunk;
	}
}

