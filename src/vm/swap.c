#include "vm/swap.h"
#include <stdio.h>
#include "userprog/syscall.h"
#include "threads/malloc.h"
#include <inttypes.h>

#define ERROR_CODE -1
#define DEBUG 0

void add_swap_entry_helper(int offset, void* vaddr);

void init_swap_table(void)
{
	if(DEBUG)
		printf("initializing swap table....");
	swap_block = block_get_role(BLOCK_SWAP);

	uint32_t sector_count = block_size (swap_block);
	entries = ((uint32_t)sector_count)/ ((uint32_t)SECTORS_PER_PAGE);

	if(!swap_block)
		exit(ERROR_CODE);

	if(DEBUG)
		printf("block %s gotten with %d sectors which is %d pages\n", block_name(swap_block), sector_count, entries);

	swap_table = calloc(sizeof(struct Swap_entry)*entries, 1);
}

bool add_swap_entry(void* vaddr)
{
	int i;
	for (i = 0; i < entries; ++i)
	{
		if(swap_table[i].tid == NULL) {
			swap_table[i].tid = thread_current()->tid;
			swap_table[i].vaddr = vaddr;
			add_swap_entry_helper(i, vaddr);
			return true;
		}
	}
	return false;
}

void add_swap_entry_helper(int offset, void* vaddr)
{
	int i;
	for(i = 0; i < SECTORS_PER_PAGE; ++i) {
		block_write (swap_block, offset*SECTORS_PER_PAGE + i, vaddr);
		vaddr += BLOCK_SECTOR_SIZE;
	}
}
