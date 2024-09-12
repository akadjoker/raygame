#include "pch.h"
#include "Arena.hpp"
#include "Utils.hpp"

static const u32 chunkSize = 16 * 1024;
static const u32 maxBlockSize = 640;
static const u32 chunkArrayIncrement = 128;

// These are the supported object sizes. Actual allocations are rounded up the next size.
static const u32 blockSizes[blockSizeCount] =
	{
		16,	 // 0
		32,	 // 1
		64,	 // 2
		96,	 // 3
		128, // 4
		160, // 5
		192, // 6
		224, // 7
		256, // 8
		320, // 9
		384, // 10
		448, // 11
		512, // 12
		640, // 13
};



void *ArenaAlloc(u32 size)
{
	void *mem = malloc(size);

	return mem;
}

void ArenaFree(void *mem)
{
	
	free(mem);
}



struct SizeMap
{
	SizeMap()
	{
		u32 j = 0;
		values[0] = 0;
		for (u32 i = 1; i <= maxBlockSize; ++i)
		{
			assert(j < blockSizeCount);
			if (i <= blockSizes[j])
			{
				values[i] = (u8)j;
			}
			else
			{
				++j;
				values[i] = (u8)j;
			}
		}
	}

	u8 values[maxBlockSize + 1];
};

static const SizeMap sizeMap;

struct ArenaChunk
{
	u32 blockSize;
	ArenaBlock *blocks;
};

struct ArenaBlock
{
	ArenaBlock *next;
};

BlockArena::BlockArena()
{
	assert(blockSizeCount < UCHAR_MAX);

	m_chunkSpace = chunkArrayIncrement;
	m_chunkCount = 0;
	m_chunks = (ArenaChunk *)ArenaAlloc(m_chunkSpace * sizeof(ArenaChunk));

	memset(m_chunks, 0, m_chunkSpace * sizeof(ArenaChunk));
	memset(m_freeLists, 0, sizeof(m_freeLists));
	m_allocation = 0;
	m_deallocation = 0;
	m_total = 0;
}

BlockArena::~BlockArena()
{
	for (u32 i = 0; i < m_chunkCount; ++i)
	{
		ArenaFree(m_chunks[i].blocks);
	}

	ArenaFree(m_chunks);
}

void *BlockArena::Allocate(u32 size)
{
	if (size == 0)
	{
		return nullptr;
	}

	assert(0 < size);
	m_total += size;
	m_allocation += size;

	if (size > maxBlockSize)
	{
		return ArenaAlloc(size);
	}

	u32 index = sizeMap.values[size];
	assert(0 <= index && index < blockSizeCount);

	if (m_freeLists[index])
	{
		ArenaBlock *block = m_freeLists[index];
		m_freeLists[index] = block->next;
		return block;
	}
	else
	{
		if (m_chunkCount == m_chunkSpace)
		{
			ArenaChunk *oldChunks = m_chunks;
			m_chunkSpace += chunkArrayIncrement;
			m_chunks = (ArenaChunk *)ArenaAlloc(m_chunkSpace * sizeof(ArenaChunk));
			memcpy(m_chunks, oldChunks, m_chunkCount * sizeof(ArenaChunk));
			memset(m_chunks + m_chunkCount, 0, chunkArrayIncrement * sizeof(ArenaChunk));
			ArenaFree(oldChunks);
		}

		ArenaChunk *chunk = m_chunks + m_chunkCount;
		chunk->blocks = (ArenaBlock *)ArenaAlloc(chunkSize);
#if defined(_DEBUG)
		memset(chunk->blocks, 0xcd, chunkSize);
#endif
		u32 blockSize = blockSizes[index];
		chunk->blockSize = blockSize;
		u32 blockCount = chunkSize / blockSize;
		assert(blockCount * blockSize <= chunkSize);
		for (u32 i = 0; i < blockCount - 1; ++i)
		{
			ArenaBlock *block = (ArenaBlock *)((c8 *)chunk->blocks + blockSize * i);
			ArenaBlock *next = (ArenaBlock *)((c8 *)chunk->blocks + blockSize * (i + 1));
			block->next = next;
		}
		ArenaBlock *last = (ArenaBlock *)((c8 *)chunk->blocks + blockSize * (blockCount - 1));
		last->next = nullptr;

		m_freeLists[index] = chunk->blocks->next;
		++m_chunkCount;

		return chunk->blocks;
	}
}

void BlockArena::Free(void *p, u32 size)
{
	if (size == 0)
	{
		return;
	}

	assert(0 < size);

	m_deallocation += size;
	m_total -= size;

	if (size > maxBlockSize)
	{
		ArenaFree(p);
		return;
	}

	u32 index = sizeMap.values[size];
	assert(0 <= index && index < blockSizeCount);

#if defined(_DEBUG)
	// Verify the memory address and size is valid.
	u32 blockSize = blockSizes[index];
	bool found = false;
	for (u32 i = 0; i < m_chunkCount; ++i)
	{
		ArenaChunk *chunk = m_chunks + i;
		if (chunk->blockSize != blockSize)
		{
			assert((c8 *)p + blockSize <= (c8 *)chunk->blocks ||
				   (c8 *)chunk->blocks + chunkSize <= (c8 *)p);
		}
		else
		{
			if ((c8 *)chunk->blocks <= (c8 *)p && (c8 *)p + blockSize <= (c8 *)chunk->blocks + chunkSize)
			{
				found = true;
			}
		}
	}

	assert(found);

	memset(p, 0xfd, blockSize);
#endif

	ArenaBlock *block = (ArenaBlock *)p;
	block->next = m_freeLists[index];
	m_freeLists[index] = block;
}

void BlockArena::Clear()
{

	m_chunkCount = 0;
	memset(m_chunks, 0, m_chunkSpace * sizeof(ArenaChunk));
	memset(m_freeLists, 0, sizeof(m_freeLists));
}

StackArena::StackArena()
{
	m_index = 0;
	m_allocation = 0;
	m_maxAllocation = 0;
	m_entryCount = 0;
}

StackArena::~StackArena()
{
	assert(m_index == 0);
	assert(m_entryCount == 0);
}

void *StackArena::Allocate(u32 size)
{
	assert(m_entryCount < maxStackEntries);

	StackEntry *entry = m_entries + m_entryCount;
	entry->size = size;
	if (m_index + size > stackSize)
	{
		entry->data = (char *)ArenaAlloc(size);
		entry->usedMalloc = true;
	}
	else
	{
		entry->data = m_data + m_index;
		entry->usedMalloc = false;
		m_index += size;
	}

	m_allocation += size;
	m_maxAllocation = std::max(m_maxAllocation, m_allocation);
	++m_entryCount;

	return entry->data;
}

void StackArena::Free(void *p)
{
	assert(m_entryCount > 0);
	StackEntry *entry = m_entries + m_entryCount - 1;
	assert(p == entry->data);
	if (entry->usedMalloc)
	{
		ArenaFree(p);
	}
	else
	{
		m_index -= entry->size;
	}
	m_allocation -= entry->size;
	--m_entryCount;

	p = nullptr;
}

u32 StackArena::GetMaxAllocation() const
{
	return m_maxAllocation;
}