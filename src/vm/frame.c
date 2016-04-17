#include <stdio.h>
#include "vm/frame.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "vm/page.h"
#include "userprog/syscall.h"
#include "userprog/pagedir.h"

#define DEBUG 0
#define ERROR_CODE -1

static int max_frame;
static uint32_t bump_ptr;

uint8_t * evict_page(void);

struct FrameTableEntry* alloc_frame_table(void)
{
	int count = 1;
	
	void* last_ptr = (void*) palloc_get_page (PAL_USER);
	void* page_ptr = (void*) palloc_get_page (PAL_USER);
	void* first_ptr = last_ptr;
	while (page_ptr)
	{
		ASSERT ((page_ptr - last_ptr) == PGSIZE);
		last_ptr = page_ptr;
		count++;
		page_ptr = palloc_get_page (PAL_USER);
	}
	max_frame = count;

	struct FrameTableEntry* ft_ptr = (struct FrameTableEntry*)malloc (count * sizeof (struct FrameTableEntry));

	int i = 0;

	for(i; i < count; ++i)
	{
		ft_ptr[i].frame_ptr = first_ptr + i * PGSIZE;
		ft_ptr[i].status = FT_EMPTY;
	}

	return ft_ptr;
}

void init_frame_table(void)
{
	sema_init (&paging_sema, 0);
	frame_table = alloc_frame_table ();
}

uint8_t* get_user_page(void * vaddr)
{
	int total = 0;
	while (frame_table[bump_ptr].status == FT_FULL)
	{
		++bump_ptr;
		++total;
		if (bump_ptr == max_frame)
			bump_ptr = 0;
		if (total == max_frame)
		{
			if(DEBUG){
				printf("bumpy bumperson: %d \n", bump_ptr);
			}
			return evict_page();
		}
	}
	frame_table[bump_ptr].status = FT_FULL;
	frame_table[bump_ptr].vaddr = vaddr;
	return frame_table[bump_ptr].frame_ptr;
}

uint8_t* evict_page(void)
{
	struct thread* t = thread_current ();
	if (DEBUG)
		printf ("thread_current() name: %s\n", t->name);


	//Finding page to evict using accessed bit
	while (pagedir_is_accessed (t->pagedir, frame_table[bump_ptr].vaddr))
	{
		pagedir_set_accessed (t->pagedir, frame_table[bump_ptr].vaddr, false);
		++bump_ptr;
		if(bump_ptr == max_frame){
			bump_ptr = 0;
		}
	}

	//Emptying frame table entry
	frame_table[bump_ptr].status = FT_EMPTY;

	if(DEBUG){
		printf("ft: %p  bump_ptr: %d  vaddr: %p\n", frame_table, bump_ptr, frame_table[bump_ptr].vaddr);
	}

	//Updating SPT_entry info
	struct SPT_entry *spte = get_SPT_entry(frame_table[bump_ptr].vaddr);
	// dies above ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	spte->resident_bit = false;
	//Swapping all stack/dirty pages to disk

	if(DEBUG){
		
		//printf("pagedir: %p ", t->pagedir);
		printf("vaddr: %p ", frame_table[bump_ptr].vaddr);
		printf("stacky: %p \n", spte->is_stack_page);
	}
	if (pagedir_is_dirty (t->pagedir, frame_table[bump_ptr].vaddr) || spte->is_stack_page)
	{
		if (DEBUG) {
			if (spte->is_stack_page)
				printf("Moving stack page to swap space\n");
		 	else
		 		printf("Moving dirty page to swap space\n");
		 }

		if(pagedir_is_dirty (t->pagedir, frame_table[bump_ptr].vaddr))
			spte->dirty_bit = true;

		spte->swap_index = add_swap_entry(frame_table[bump_ptr].vaddr);
		if (spte->swap_index == -1) {
			if (DEBUG)
				printf("ERROR adding swap entry\n");
			exit(ERROR_CODE);
		}
	}

	//Updating page directory entry
	pagedir_clear_page (t->pagedir, frame_table[bump_ptr].vaddr);

	return frame_table[bump_ptr].frame_ptr;
}

uint32_t frameptr_to_frame_num(void* addr)
{
	return ((int)addr-(int)frame_table[0].frame_ptr)/PGSIZE;
}
void* frame_num_to_frameptr (uint32_t frame_num){
	ASSERT (frame_num <= max_frame);
	return (void*)(PGSIZE*frame_num + (uint32_t)frame_table[0].frame_ptr);
}
