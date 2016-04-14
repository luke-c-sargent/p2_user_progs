//Supplemental Page Table
#include "lib/kernel/bitmap.h"
#include "lib/kernel/hash.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "vm/frame.h"
#include "threads/vaddr.h"
#include "filesys/off_t.h"

enum SPT_status {
	SPT_RESIDENT=0,
	// SPT_USED,
	// SPT_DIRTY,
	SPT_WRITABLE,
	//SPT_STACK, added helper function instead
	// IN SWAP?
	LAST_ELEMENT_UNUSED
};


// correlates to size of SPT_status and its complementary bitmap entries
static const int status_count = LAST_ELEMENT_UNUSED - SPT_RESIDENT;
static const int max_page = (((int)PHYS_BASE) / PGSIZE) - 1; //Ali: Don't think these are the same type

struct SPT{
	struct hash hash_table;
	void* stack_pointer;
};

struct SPT_entry{
	//struct bitmap *status_map;
	bool is_stack_page;
	void * vaddr;	//Ali: uint8_t????
	uint32_t page_number;
	bool resident_bit;
	off_t ofs;
	bool writable;
	struct hash_elem hash_elem;
};

//struct SPT_entry *SPT;
//add a semaphore

// function declaration
//Ali
struct SPT_entry* create_SPT_entry(void* vaddr, bool resident_bit, 
		off_t ofs, bool writable);
void remove_SPT_entry(struct SPT_entry* spte);
struct SPT* init_SPT(void);


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
