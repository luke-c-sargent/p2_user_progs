#include "vm/swap.h"
#include <stdio.h>
#include "userprog/syscall.h"
#include "threads/malloc.h"
#include <inttypes.h>

#define ERROR_CODE -1
#define DEBUG 1

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

int add_swap_entry(void* vaddr)
{
	if(DEBUG)
		printf("Searching %d swap entries for a location for vaddr %p\n", entries, vaddr);
	uint32_t i;
	for (i = 0;  i < entries; ++i)
	{

		if(swap_table[i].tid == 0) 
		{
			swap_table[i].tid = thread_current()->tid;
			swap_table[i].vaddr = vaddr;
			if(DEBUG)
				printf("adding tid %d's vaddr %p at index %d\n", swap_table[i].tid, vaddr, i);
			add_swap_entry_helper(i, vaddr);
			return i;
		}
	}
	return -1;
}

void add_swap_entry_helper(int offset, void* vaddr)
{
	uint32_t i;
	for(i = 0; i < SECTORS_PER_PAGE; ++i) {
		if(DEBUG){printf("off: %d sects: %d i: %d result: %d\n", offset, SECTORS_PER_PAGE, i, offset * SECTORS_PER_PAGE + i);}
		block_write (swap_block, offset * SECTORS_PER_PAGE + i, vaddr);
		vaddr += BLOCK_SECTOR_SIZE;
	}
}

//read and remove
void remove_swap_entry(int index, void* paddr, struct SPT_entry* spte)
{
	if(DEBUG)
		printf("Removing swap entry for paddr %p at index %d...\n", paddr, index);
	if(swap_table[index].tid == 0) 
	{
		exit (ERROR_CODE);
	}

	uint32_t i;
	//take memory and write to frame
	for(i = 0; i < SECTORS_PER_PAGE; ++i) 
	{
		if(DEBUG)
			printf("off: %d sects: %d i: %d result: %d\n", index, SECTORS_PER_PAGE, i, index * SECTORS_PER_PAGE + i);

		block_read (swap_block, index * SECTORS_PER_PAGE + i, paddr);
		paddr += BLOCK_SECTOR_SIZE;
	}
	swap_table[index].tid = 0;
	swap_table[index].vaddr = 0;
	spte->swap_index = -1;
	if(DEBUG)
		printf("... removed\n");
}
