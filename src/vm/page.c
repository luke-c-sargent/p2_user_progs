#include <stdio.h>
#include "vm/page.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"

#define DEBUG 1

//Ali: added this function
struct SPT_entry* create_SPT_entry(tid_t tid, uint32_t frame_number, void* stack_page_ptr){ // might want to pass init values here
	struct SPT_entry* temp = (struct SPT_entry*) malloc(sizeof(struct SPT_entry));
	// initialize entry values
	temp->owner_tid = tid;
	temp->frame_number = frame_number;
	temp->stack_ptr = stack_page_ptr;
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
	int digest = spte->owner_tid * 1000 + spte->frame_number;
	if(DEBUG)
		printf("HASHER: digested tid %d and frame %d to %d\n", spte->owner_tid, spte->frame_number, digest);
	ASSERT(digest > 0);
	return hash_bytes (&digest, sizeof digest);

}
// less func
// sort by TID and frame number. 
// a is > if a->owner_tid is > than b->owner_tid
// 		if a->owner_tid == b->owner_tid, a > b if a->frame_number > b->frame_number
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_,
           void *aux UNUSED){

	const struct SPT_entry *a = hash_entry (a_, struct SPT_entry, hash_elem);
	const struct SPT_entry *b = hash_entry (b_, struct SPT_entry, hash_elem);
	if(DEBUG)
		printf("LESS: comparing tids a=%d, b=%d...", a->owner_tid, b->owner_tid);
	if( a->owner_tid == b->owner_tid){
		if(DEBUG)
			printf("  ...tids were equal, comparing frames a=%d, b=%d!\n", a->frame_number, b->frame_number);
		ASSERT(a->frame_number != b->frame_number);
		return ( a->frame_number < b->frame_number );
	}
	if(DEBUG)
		printf(" ... done!\n");
 	return a->owner_tid < b->owner_tid;
}

bool is_stack_spt_entry(struct SPT_entry* spte){
	return spte->stack_ptr == frame_num_to_frameptr(spte->frame_number);
}
