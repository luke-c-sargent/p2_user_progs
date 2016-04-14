#include <stdio.h>
#include "vm/page.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"


#define DEBUG 0

//Ali: added this function
struct SPT_entry* create_SPT_entry(void* vaddr, bool resident_bit){ // might want to pass init values here
	struct SPT_entry* temp = (struct SPT_entry*) malloc(sizeof(struct SPT_entry));
	// initialize entry values
	temp->is_stack_page = false;
	temp->vaddr = vaddr;
	temp->resident_bit = resident_bit;
	// add temp to the hash using hash list
	temp->status_map = bitmap_create(status_count); // make bitmap
	// do any modifications to bitmap?
	hash_insert(&SPT, &temp->hash_elem);
	return temp;
}
// clean up SPT entry on remove from hash
void remove_SPT_entry(struct SPT_entry* spte){
	hash_delete(&SPT, &spte->hash_elem);
	bitmap_destroy(spte->status_map);
	free(spte);
}

// initialize hash 
void init_SPT(void){
	// allocate sufficient memory
	//SPT = (struct SPT_entry*) malloc(sizeof(struct SPT_entry));
	// initialize bit field
	//status_map = bitmap_create (status_count);
	if(DEBUG)
		printf("initializing SPT\n");
	// other stuff
	hash_init(&SPT, hasher, page_less, NULL);
}


// hash func
unsigned int hasher(const struct hash_elem *p_, void *aux UNUSED){
	const struct SPT_entry *spte = hash_entry (p_, struct SPT_entry, hash_elem);
	int page_number = (int)(spte->vaddr >> PGBITS);
	int digest = spte->owner_tid * 1000 + page_number;
	if(DEBUG)
		printf("HASHER: digested tid %d and page_number %d to %d\n", spte->owner_tid, page_number, digest);
	ASSERT(digest > 0);
	return hash_bytes (&digest, sizeof digest);

}

// less func
// sort by TID and page number. 
// a is > if a->owner_tid is > than b->owner_tid
// 		if a->owner_tid == b->owner_tid, a > b if a->page_number > b->page_number
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
           void *aux UNUSED){

	const struct SPT_entry *a = hash_entry (a_, struct SPT_entry, hash_elem);
	const struct SPT_entry *b = hash_entry (b_, struct SPT_entry, hash_elem);
	if(DEBUG)
		printf("LESS: comparing tids a=%d, b=%d...", a->owner_tid, b->owner_tid);
	if( a->owner_tid == b->owner_tid){
		uint32_t a_pgnum = vaddr_to_page_num(a->vaddr);
		uint32_t b_pgnum = vaddr_to_page_num(b->vaddr);
		if(DEBUG)
			printf("  ...tids were equal, comparing pages a=%d, b=%d!\n", a_pgnum, b_pgnum);
		ASSERT(a_pgnum != b_pgnum);
		return ( a_pgnum < b_pgnum );
	}
	if(DEBUG)
		printf(" ... done!\n");
 	return a->owner_tid < b->owner_tid;
}

struct SPT_entry* get_SPT_entry(tid_t tid, void* vaddr){	//Ali: change to hash find
	//struct hash_elem e = 

	struct hash_iterator i;
	struct hash_elem* e;
	hash_first (&i, &SPT);
	while ((e = hash_next (&i)))
 	{
    	struct SPT_entry *temp = hash_entry (hash_cur (&i), struct SPT_entry, hash_elem);
    	if(tid == temp->owner_tid && page_number == temp->vaddr)
    		return temp;
 	}
 	return NULL;
}


//Ali: uuuuuuuuuh
bool is_stack_spt_entry(struct SPT_entry* spte){
	return spte->stack_ptr == frame_num_to_frameptr(spte->frame_number);
}

uint32_t vaddr_to_page_num(void* addr){
	ASSERT(is_user_vaddr(addr))
	return (uint32_t)(spte->vaddr >> PGBITS);
}
void* page_num_to_vaddr(uint32_t page_number){
	ASSERT(page_number <= max_page);
	return (void*)(PGSIZE*page_number);
}

