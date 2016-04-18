#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdint.h>
#include "threads/synch.h"

enum FT_STATUS 				/* Possible status of the frame table */
{
	FT_EMPTY = 0,			/* frame is empty and can be allocated */
	FT_FULL					/* frame is full and could be evicted */
};

struct FrameTableEntry 		/* Entry of a single frame */
{
	void * frame_ptr;		/* pointer to physical memory where frame resides */
	int status;				/* status of this frame */
	void * vaddr;			/* virtual address this frame correlates to */
};

struct FrameTableEntry* frame_table; /* pointer to first frame table entry for indexing */
struct semaphore paging_sema;	/* semaphore for atomic access */

// function declarations
struct FrameTableEntry* alloc_frame_table (void);
void init_frame_table (void);
uint8_t * get_user_page (void * vaddr);
uint32_t frameptr_to_frame_num (void* addr);
void* frame_num_to_frameptr (uint32_t frame_num);
#endif /* vm/frame.h */
