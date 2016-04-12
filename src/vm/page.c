#include <stdio.h>
#include "vm/page.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"

//Ali: added this function
struct SPT_entry* create_SPT_entry(void){
	struct SPT_entry* temp = (struct SPT_entry*) malloc(sizeof(struct SPT_entry));
	// add temp to the hash using hash list
	return temp;
}

//Ali: have we put this in thread_init?
void init_SPT(void){
	// allocate sufficient memory
	SPT = (struct SPT_entry*) malloc(sizeof(struct SPT_entry));
	// initialize bit field
	bitmap_create (size_t bit_cnt)

	// other stuff
}
