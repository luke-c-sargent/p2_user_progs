#include <stdio.h>
#include "vm/page.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"

#define DEBUG 0
#define SUPERDEBUG 0

struct hash* get_curr_hash ()
{
	return &(thread_current ()->SP_table.hash_table);
}

//Ali: added this function
/* 	Function: create_SPT_entry
	Description: creates a SPT entry
	Parameters: virtual address pointer, boolean resident bit value, offset, bytes 
				of page read, bytes of page to zero out, whether page is writable
	Returns: created supplemental page table entry
*/
struct SPT_entry* create_SPT_entry (void* vaddr, bool resident_bit, 
		 off_t ofs, size_t page_read_bytes, size_t page_zero_bytes, bool writable)
{
	struct SPT_entry* temp = (struct SPT_entry*) calloc (sizeof (struct SPT_entry), 1);
	// initialize entry values
	temp->is_stack_page = false;
	temp->vaddr = vaddr;
	temp->page_number = vaddr_to_page_num (vaddr);
	temp->resident_bit = resident_bit;
	temp->swap_index = 0;
	temp->dirty_bit = false;
	temp->ofs = ofs;
	temp->page_read_bytes = page_read_bytes;
	temp->page_zero_bytes = page_zero_bytes;
	temp->writable = writable;
	
	// add temp to the hash using hash list
	if (DEBUG)
		printf ("inserting into hash %p with elem %p \n", get_curr_hash(), &temp->hash_elem);
	hash_insert (get_curr_hash (), &temp->hash_elem);
	ASSERT( get_SPT_entry (vaddr));
	if (DEBUG)
		printf ("inserted!\n");
	return temp;
}

/* 	Function: remove_SPT_entry
	Description: clean up SPT entry on remove from hash
	Parameters: SPT entry struct pointer
	Returns: void
*/
void remove_SPT_entry (struct SPT_entry* spte)
{
	hash_delete (get_curr_hash (), &spte->hash_elem);
	free (spte);
}

/* 	Function: init_SPT
	Description: initialize hash 
	Parameters: thread struct pointer
	Returns: void
*/
void init_SPT (struct thread* t)
{
	if (DEBUG)
		printf ("initializing SPT\n");
	struct SPT temp = t->SP_table;
}

/* 	Function: hasher
	Description: hash function
	Parameters: hash element struct pointer, ability to pass additional variable parameters
	Returns: unsigned int of page number digested
*/
unsigned int hasher (const struct hash_elem *p_, void *aux UNUSED)
{
	const struct SPT_entry *spte = hash_entry (p_, struct SPT_entry, hash_elem);
	unsigned digest = hash_int ((int) spte->page_number);
	if (DEBUG && SUPERDEBUG)
		printf ("HASHER: digested page_number %d to %d\n", spte->page_number, digest);
	ASSERT(digest > 0);
	return digest;
}


/* 	Function: page_less
	Description: sorts a page by a comparing SPT entry page numbers
				a < b if a->page_number < b->page_number
	Parameters: hash element struct pointer, hash element struct pointer, ability to pass in additional parameters
	Returns: boolean of a->page_number is less than b->page_number
*/
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
           void *aux UNUSED)
{

	const struct SPT_entry *a = hash_entry (a_, struct SPT_entry, hash_elem);
	const struct SPT_entry *b = hash_entry (b_, struct SPT_entry, hash_elem);
	if (DEBUG && SUPERDEBUG)
	{
		printf ("comparing pages a=%d, b=%d!\n", a->page_number, b->page_number);
	}
 	return a->page_number < b->page_number;
}

/* 	Function: get_SPT_entry
	Description: uses a virtual address to get an SPT entry
	Parameters: virtual address
	Returns: hash struct of SPT entry
*/
struct SPT_entry* get_SPT_entry(void* vaddr)
{
	uint32_t page_number = vaddr_to_page_num (vaddr);
	struct SPT_entry spte;
	spte.page_number = page_number;
	struct hash_elem* e = hash_find (get_curr_hash (), &spte.hash_elem);
	return e != NULL ? hash_entry (e, struct SPT_entry, hash_elem) : NULL;
}

/* 	Function: vaddr_to_page_num
	Description: translate a virtual address to a page number
	Parameters: virtual address
	Returns: uint32_t page number
*/
uint32_t vaddr_to_page_num (void* addr)
{
	if (!is_user_vaddr(addr))
		exit(-1);
	uint32_t page_number = ((uint32_t) addr) >> PGBITS;
	ASSERT (page_number < max_page);
	if (DEBUG)
		printf ("max_page = %u\n", max_page);
	return page_number;
}

/* 	Function: page_num_to_vaddr
	Description: translate a page number to a virtual address
	Parameters: page number
	Returns: void pointer to virtual address
*/
void* page_num_to_vaddr (uint32_t page_number)
{
	ASSERT (page_number < max_page);
	void* vaddr = (void*)(PGSIZE*page_number);
	ASSERT (is_user_vaddr (vaddr));
	if (DEBUG)
		printf ("max_page = %u\n", max_page);
	return vaddr;
}
