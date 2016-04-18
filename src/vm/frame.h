#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdint.h>
#include "threads/synch.h"

enum FT_STATUS 				/* Possible status of the frame table */
{
	FT_EMPTY = 0,
	FT_FULL
};

struct FrameTableEntry 		/* Entry of a single frame */
{
	void * frame_ptr;		
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
