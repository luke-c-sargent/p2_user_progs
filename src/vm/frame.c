#include <stdio.h>
#include "vm/frame.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"

struct FrameTableEntry* alloc_frame_table(){
	int count = 1;
	void* page_ptr = 1;
	void* last_ptr = 0;
	last_ptr = palloc_get_page(PAL_USER);
	page_ptr = palloc_get_page(PAL_USER);
	void* first_ptr = last_ptr;
	while(page_ptr){
		ASSERT((page_ptr - last_ptr) == PGSIZE);
		last_ptr = page_ptr;
		count++;
		page_ptr = palloc_get_page (PAL_USER);
	}
	return (struct FrameTableEntry*)malloc(count*sizeof(struct FrameTableEntry));
}

void init_frame_table(){
	frame_table = alloc_frame_table();
}

