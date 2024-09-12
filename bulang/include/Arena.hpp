#pragma once

#include "Config.hpp"



const u32 stackSize = 100 * 1024;	// 100k
const u32 maxStackEntries = 256;
const u32 blockSizeCount = 64;//14

struct ArenaBlock;
struct ArenaChunk;

void *ArenaAlloc(u32 size);

void ArenaFree(void *p);

struct  StackEntry
{
	char* data;
	u32 size;
	bool usedMalloc;
};



class  BlockArena
{
public:
	BlockArena();
	~BlockArena();


	void* Allocate(u32 size);


	void Free(void* p, u32 size);

	void Clear();

	u32  size() const { return m_total; }

private:

	ArenaChunk* m_chunks;
	u32 m_chunkCount;
	u32 m_chunkSpace;
	u32 m_total;
	u32 m_allocation;
	u32 m_deallocation;

	ArenaBlock* m_freeLists[blockSizeCount];
};




class  StackArena
{
public:
	StackArena();
	~StackArena();

	void* Allocate(u32 size);
	void Free(void* p);

	u32 GetMaxAllocation() const;

private:

	char m_data[stackSize];
	u32 m_index;

	u32 m_allocation;
	u32 m_maxAllocation;

	StackEntry m_entries[maxStackEntries];
	u32 m_entryCount;
};
