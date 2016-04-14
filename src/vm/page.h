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
static const int max_page = ((int)PHYS_BASE) / PGSIZE; //Ali: Don't think these are the same type

struct SPT_entry{
	struct bitmap *status_map;
	tid_t owner_tid;
	bool is_stack_page;
	void * vaddr;	//Ali: uint8_t????
	bool resident_bit;
	// PT ENTRY?
	//file name
	//offset
	struct hash_elem hash_elem;
};

//struct SPT_entry *SPT;
//add a semaphore

// function declaration
//Ali
struct SPT_entry* create_SPT_entry(void* vaddr, bool resident_bit);
void remove_SPT_entry(struct SPT_entry* spte);
void init_SPT(void);


// hash func
unsigned int hasher(const struct hash_elem *p_, void *aux UNUSED);

// less func
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
           void *aux UNUSED);

struct SPT_entry* get_SPT_entry(tid_t tid, void* vaddr);



bool is_stack_spt_entry(struct SPT_entry* spte);
uint32_t vaddr_to_page_num(void* addr);
void* page_num_to_vaddr(uint32_t page_number);

/* Loads a segment starting at offset OFS in FILE at address
   UPAGE.  In total, READ_BYTES + ZERO_BYTES bytes of virtual
   memory are initialized, as follows:

        - READ_BYTES bytes at UPAGE must be read from FILE
          starting at offset OFS.

        - ZERO_BYTES bytes at UPAGE + READ_BYTES must be zeroed.

   The pages initialized by this function must be writable by the
   user process if WRITABLE is true, read-only otherwise.

   Return true if successful, false if a memory allocation error
   or disk read error occurs. 
static bool
load_segment (struct file *file, off_t ofs, uint8_t *upage,
              uint32_t read_bytes, uint32_t zero_bytes, bool writable) 

              */

// 
