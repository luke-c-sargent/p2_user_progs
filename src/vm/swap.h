#ifndef SWAP_H
#define SWAP_H

#include "threads/thread.h"
#include "devices/block.h"

#define SECTORS_PER_PAGE PGSIZE/BLOCK_SECTOR_SIZE

struct Swap_entry
{
	tid_t tid; // owning thread's TID
	void* vaddr; // owning page's virtual address
	// block stuff
};

// malloc this by block size
struct Swap_entry* swap_table;

static struct block* swap_block;

static uint32_t entries;

void init_swap_table(void);

bool add_swap_entry(void* vaddr);

// void remove_swap_entry();

// void get_swap_entry();

#endif /* vm/swap.h */
