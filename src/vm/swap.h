#ifndef SWAP_H
#define SWAP_H

#include "threads/thread.h"
#include "devices/block.h"
#include "vm/page.h"

#define SECTORS_PER_PAGE PGSIZE/BLOCK_SECTOR_SIZE

struct Swap_entry
{
	tid_t tid; 		/* owning thread's TID */
	void* vaddr; 	/* owning page's virtual address */
};

struct Swap_entry* swap_table; /* pointer to first element of swap table for random access */

static struct block* swap_block; 	/* swap block pointer for meta-data access */
static uint32_t entries; 			/* number of entries in swap table to avoid out-of-bounds access */

void init_swap_table(void);
int add_swap_entry(void* vaddr);
void remove_swap_entry(int index, void* paddr, struct SPT_entry * spte);

#endif /* vm/swap.h */
