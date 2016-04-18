#ifndef VM_PAGE_H
#define VM_PAGE_H

#include "lib/kernel/bitmap.h"
#include "lib/kernel/hash.h"
#include "threads/synch.h"
#include "vm/frame.h"
#include "threads/vaddr.h"
#include "filesys/off_t.h"


static const uint32_t max_page = ((uint32_t) PHYS_BASE / ((uint32_t) PGSIZE));

struct SPT{
	struct hash hash_table;			/* underlying hash table in wrapper */
};

#include "threads/thread.h"

struct SPT_entry {
	bool is_stack_page;				/* flag indicating stack page */
	void * vaddr;					/* the virtual address of the entry */
	uint32_t page_number;			/* the page number calculated from virtual address */
	bool resident_bit;				/* flag indicating if page is resident in physical memory */
	int swap_index;					/* index of page in swap table / swap partition */
	bool dirty_bit;					/* flag indicating if page has been written to */
	off_t ofs;						/* file offset for loading from file to frame */
	size_t page_read_bytes;			/* number of data bytes to read from file to page */
	size_t page_zero_bytes;			/* bytes that need to be zeroed at end of page */
	bool writable;					/* if the page can be written to */
	struct hash_elem hash_elem;		/* hash element for hash table indexing/retrieval */
};

// function declaration
struct SPT_entry* create_SPT_entry (void* vaddr, bool resident_bit, 
		off_t ofs, size_t page_read_bytes, size_t page_zero_bytes, bool writable);
void remove_SPT_entry (struct SPT_entry* spte);
void init_SPT (struct thread* t);
unsigned int hasher (const struct hash_elem *p_, void *aux UNUSED);
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
           void *aux UNUSED);
struct SPT_entry* get_SPT_entry (void* vaddr);
uint32_t vaddr_to_page_num (void* addr);
void* page_num_to_vaddr (uint32_t page_number);
#endif /* vm/page.h */
