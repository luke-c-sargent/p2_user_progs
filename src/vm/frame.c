#include <stdio.h>
#include "vm/frame.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"

static int max_frame;
static int bump_ptr;

struct FrameTableEntry* alloc_frame_table(void){
	int count = 1;
	
	void* last_ptr = (void*) palloc_get_page(PAL_USER);;
	void* page_ptr = (void*) palloc_get_page(PAL_USER);;
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

void init_frame_table(void){
	sema_init(&paging_sema, 0);
	frame_table = alloc_frame_table();
}

uint8_t * get_user_page(void){	//Ali: UPDATE PTE
	int total = 0;
	while (frame_table[bump_ptr].status == FT_FULL){
		++bump_ptr;
		++total;
		if(bump_ptr == max_frame)
			bump_ptr = 0;
		if(total == max_frame){
			break;
			// evict instead of break, get bump_ptr to evicted mem, update status
		}
	}
	frame_table[bump_ptr].status = FT_FULL;
	return frame_table[bump_ptr].frame_ptr;
}

uint32_t frameptr_to_frame_num(void* addr){
	return ((int)addr-(int)frame_table[0].frame_ptr)/PGSIZE;
}
void* frame_num_to_frameptr(uint32_t frame_num){
	ASSERT(frame_num <= max_frame);
	return (void*)(PGSIZE*frame_num + (uint32_t)frame_table[0].frame_ptr);
}
