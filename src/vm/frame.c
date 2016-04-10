#include <stdio.h>
#include "vm/frame.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"

static int max_frame;

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
	max_frame = count;

	struct FrameTableEntry* ft_ptr = (struct FrameTableEntry*)malloc(count*sizeof(struct FrameTableEntry));

	int i = 0;

	for(i; i < count; ++i) {
		ft_ptr[i].frame_ptr = first_ptr + i * PGSIZE;
		ft_ptr[i].status = FT_EMPTY;
	}

	return ft_ptr;
}

void init_frame_table(){
	sema_init(&FT_sema, 0);
	frame_table = alloc_frame_table();
}