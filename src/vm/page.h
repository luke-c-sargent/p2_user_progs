#ifndef VM_PAGE_H
#define VM_PAGE_H

//Supplemental Page Table
#include "lib/kernel/bitmap.h"
#include "lib/kernel/hash.h"
#include "threads/synch.h"
#include "vm/frame.h"
#include "threads/vaddr.h"
#include "filesys/off_t.h"

/*
enum SPT_status {
	SPT_RESIDENT=0,
	// SPT_USED,
	// SPT_DIRTY,
	SPT_WRITABLE,
	//SPT_STACK, added helper function instead
	// IN SWAP?
	LAST_ELEMENT_UNUSED
};
*/

// correlates to size of SPT_status and its complementary bitmap entries
// static const int status_count = LAST_ELEMENT_UNUSED - SPT_RESIDENT;
static const uint32_t max_page = ((uint32_t) PHYS_BASE / ((uint32_t) PGSIZE));

struct SPT{
	struct hash hash_table;
	//void* stack_pointer; //MOVE TO THREAD STRUCT
};
// ------------------------------
#include "threads/thread.h"
// ------------------------------
struct SPT_entry{
	//struct bitmap *status_map;
	bool is_stack_page;
	void * vaddr;
	uint32_t page_number;
	bool resident_bit;
	off_t ofs;
	size_t page_read_bytes;
	size_t page_zero_bytes;
	bool writable;
	struct hash_elem hash_elem;
};

//struct SPT_entry *SPT;
//add a semaphore

// function declaration
//Ali
struct SPT_entry* create_SPT_entry(void* vaddr, bool resident_bit, 
		off_t ofs, size_t page_read_bytes, size_t page_zero_bytes, bool writable);
void remove_SPT_entry(struct SPT_entry* spte);
void init_SPT(struct thread* t);


// hash func
unsigned int hasher(const struct hash_elem *p_, void *aux UNUSED);

// less func
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
           void *aux UNUSED);

struct SPT_entry* get_SPT_entry(void* vaddr);
uint32_t vaddr_to_page_num(void* addr);
void* page_num_to_vaddr(uint32_t page_number);

// check get_SPT_entry?
// how to get VA of stack_pointer? is top of VA = PHYS_BASE - PGSIZE
#endif /* vm/page.h */
