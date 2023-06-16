#include "Precompiled.h"
#include "BlockAllocator.h"

#include "DebugUtil.h"

using namespace SAGE::Core;

BlockAllocator::BlockAllocator(std::string name, size_t blockSize, size_t capacity)
	: mName(std::move(name))
	, mData(nullptr), mBlockSize(blockSize)
	, mCapacity(capacity), mFreeBlocks(capacity)
{
	ASSERT(blockSize > 0, "BlockAllocator --- Invalid block size!");
	ASSERT(capacity > 0, "BlockAllocator --- Invalid capacity!");

	// Preallocate memory
	mData = std::malloc(blockSize * capacity);

	// Generate free blocks
	for (std::size_t i = 0; i < capacity; ++i) {
		mFreeBlocks[i] = static_cast<uint8_t*>(mData) + (i * mBlockSize);
	}

	if (!mName.empty()) {
		LOG("%s allocated %zu blocks, block size: %zu", mName.c_str(), capacity, blockSize);
	}
}

BlockAllocator::BlockAllocator(size_t blockSize, size_t capacity)
	: BlockAllocator("", blockSize, capacity)
{
}

BlockAllocator::~BlockAllocator()
{
	ASSERT(mFreeBlocks.size() == mCapacity, "BlockAllocator --- Not all blocks are freed. Potential memory leak!");
	std::free(mData);

	if (!mName.empty()) {
		LOG("%s destructed. Allocated:: %zu, Freed: %zu", mName.c_str(), mBlockAllocated, mBlockFreed);
	}
}

void* BlockAllocator::Allocate()
{
	if (mFreeBlocks.empty()) {
		if (!mName.empty()) {
			LOG("%s no free locks left.", mName.c_str());
		}
		return nullptr;
	}

	// Get the next free block
	void* freeBlock = mFreeBlocks.back();
	mFreeBlocks.pop_back();

	if (!mName.empty()) {
		LOG("%s::allocate %p", mName.c_str(), freeBlock);
		++mBlockAllocated;
	}

	return freeBlock;
}

void BlockAllocator::Free(void* ptr)
{
	if (ptr == nullptr) { return; }

	const auto start = static_cast<uint8_t*>(mData);
	const auto end = static_cast<uint8_t*>(mData) + (mBlockSize * mCapacity);
	const auto current = static_cast<uint8_t*>(ptr);
	const auto diff = current - start;

	ASSERT(
		current >= start && 
		current < end &&
		static_cast<std::size_t>(diff) % mBlockSize == 0,
		"BlockAllocator --- Invalid address being freed."
	);

	if (!mName.empty()) {
		LOG("%s::Free %p", mName.c_str(), ptr);
		++mBlockFreed;
	}

	mFreeBlocks.emplace_back(ptr);
}