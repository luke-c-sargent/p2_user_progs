//Supplemental Page Table
#include "lib/kernel/bitmap.h"
#include "lib/kernel/hash.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "vm/frame.h"

enum SPT_status {
	SPT_RESIDENT=0,
	// SPT_USED,
	// SPT_DIRTY,
	SPT_WRITABLE,
	//SPT_STACK, added helper function instead
	// IN SWAP?
	LAST_ELEMENT_UNUSED
};

struct hash SPT;
// correlates to size of SPT_status and its complementary bitmap entries
static const int status_count = LAST_ELEMENT_UNUSED - SPT_RESIDENT;

struct SPT_entry{
	struct bitmap *status_map;
	int frame_number;// is this necessary? we could store pointer instead
	tid_t owner_tid;
	uint8_t *stack_ptr; //Ali: uint32_t? ---->  no, its byte-addressable
	// PT ENTRY?
	//file name
	//offset
	struct hash_elem hash_elem;
};

//struct SPT_entry *SPT;
//add a semaphore

// function declaration
//Ali
struct SPT_entry* create_SPT_entry(tid_t tid, uint32_t frame_number, void* stack_page_ptr);
void remove_SPT_entry(struct SPT_entry* spte);
void init_SPT(void);


// hash func
unsigned int hasher(const struct hash_elem *p_, void *aux UNUSED);

struct SPT_entry* get_SPT_entry(tid_t tid, void* paddr);

// less func
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
           void *aux UNUSED);

bool is_stack_spt_entry(struct SPT_entry* spte);
