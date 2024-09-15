#pragma once

#include "Config.hpp"



const s32 stackSize = 100 * 1024;	// 100k
const s32 maxStackEntries = 256;
const s32 blockSizeCount = 64;//14

struct ArenaBlock;
struct ArenaChunk;

void *ArenaAlloc(s32 size);

void ArenaFree(void *p);

struct  StackEntry
{
	char* data;
	s32 size;
	bool usedMalloc;
};



class  BlockArena
{
public:
	BlockArena();
	~BlockArena();


	void* Allocate(s32 size);


	void Free(void* p, s32 size);

	void Clear();

	s32  size() const { return m_total; }

private:

	ArenaChunk* m_chunks;
	s32 m_chunkCount;
	s32 m_chunkSpace;
	s32 m_total;
	s32 m_allocation;
	s32 m_deallocation;

	ArenaBlock* m_freeLists[blockSizeCount];
};




class  StackArena
{
public:
	StackArena();
	~StackArena();

	void* Allocate(s32 size);
	void Free(void* p);

	s32 GetMaxAllocation() const;

private:

	char m_data[stackSize];
	s32 m_index;

	s32 m_allocation;
	s32 m_maxAllocation;

	StackEntry m_entries[maxStackEntries];
	s32 m_entryCount;
};
