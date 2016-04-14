#include <stdio.h>
#include "vm/page.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"


#define DEBUG 0

struct hash* get_curr_hash(){
	return &(thread_current()->SP_table->hash_table);
}

//Ali: added this function
struct SPT_entry* create_SPT_entry(void* vaddr, bool resident_bit, 
		 off_t ofs, bool writable){ // might want to pass init values here
	struct SPT_entry* temp = (struct SPT_entry*) malloc(sizeof(struct SPT_entry));
	// initialize entry values
	temp->is_stack_page = false;
	temp->vaddr = vaddr;
	temp->page_number = vaddr_to_page_num(vaddr);
	temp->resident_bit = resident_bit;
	temp->ofs = ofs;
	temp->writable = writable;
	// add temp to the hash using hash list
	// temp->status_map = bitmap_create(status_count); // make bitmap
	// do any modifications to bitmap?
	hash_insert(get_curr_hash(), &temp->hash_elem);
	return temp;
}
// clean up SPT entry on remove from hash
void remove_SPT_entry(struct SPT_entry* spte){
	hash_delete(get_curr_hash(), &spte->hash_elem);
	free(spte);
}

// initialize hash 
struct SPT* init_SPT(void){
	if(DEBUG)
		printf("initializing SPT\n");
	struct SPT* temp = (struct SPT*) malloc(sizeof(struct SPT));
	hash_init(&(temp->hash_table), hasher, page_less, NULL);
	temp->stack_pointer = NULL;
	return temp;
}


// hash func
unsigned int hasher(const struct hash_elem *p_, void *aux UNUSED){
	const struct SPT_entry *spte = hash_entry (p_, struct SPT_entry, hash_elem);
	unsigned digest = hash_int((int) spte->page_number);
	if(DEBUG)
		printf("HASHER: digested page_number %d to %d\n", spte->page_number, digest);
	ASSERT(digest > 0);
	return digest;
}

// less func
// sort by page number. 
// a < b if a->page_number < b->page_number
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
           void *aux UNUSED){

	const struct SPT_entry *a = hash_entry (a_, struct SPT_entry, hash_elem);
	const struct SPT_entry *b = hash_entry (b_, struct SPT_entry, hash_elem);
	if(DEBUG) {
		printf("comparing pages a=%d, b=%d!\n", a->page_number, b->page_number);
		ASSERT(a->page_number != b->page_number);
	}
 	return a->page_number < b->page_number;
}

struct SPT_entry* get_SPT_entry(void* vaddr){	//Ali: change to hash find
	uint32_t page_number = vaddr_to_page_num(vaddr);
	struct SPT_entry* spte;
	spte->page_number = page_number;
	struct hash_elem* e = hash_find (get_curr_hash(), &spte->hash_elem);
	return e != NULL ? hash_entry (e, struct SPT_entry, hash_elem) : NULL;
}


uint32_t vaddr_to_page_num(void* addr){
	ASSERT(is_user_vaddr(addr));
	uint32_t page_number = ((uint32_t) addr) >> PGBITS;
	ASSERT(page_number <= max_page);
	return page_number;
}
void* page_num_to_vaddr(uint32_t page_number){
	ASSERT(page_number <= max_page);
	void* vaddr = (void*)(PGSIZE*page_number);
	ASSERT(is_user_vaddr(vaddr));
	return vaddr;
}
