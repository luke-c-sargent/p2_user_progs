#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdint.h>
#include "threads/synch.h"

// frame table
enum FT_STATUS 
{
	FT_EMPTY = 0,
	FT_FULL
};

struct FrameTableEntry 
{
	void * frame_ptr;
	uint32_t* pte;
	int status;
	void * vaddr;
};

struct FrameTableEntry* frame_table;

struct semaphore paging_sema;

// function declarations
struct FrameTableEntry* alloc_frame_table (void);
void init_frame_table (void);
uint8_t * get_user_page (void * vaddr);
uint32_t frameptr_to_frame_num (void* addr);
void* frame_num_to_frameptr (uint32_t frame_num);
#endif /* vm/frame.h */
