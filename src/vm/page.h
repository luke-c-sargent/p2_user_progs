//Supplemental Page Table
#include "lib/kernel/bitmap.h"

enum SPT_status {
	SPT_RESIDENT=0,
	// SPT_USED,
	// SPT_DIRTY,
	SPT_WRITABLE,
	SPT_STACK,
	// IN SWAP?
	LAST_ELEMENT_UNUSED
};

// correlates to size of SPT_status and its complementary bitmap entries
const int status_count = LAST_ELEMENT_UNUSED - SPT_RESIDENT;

struct SPT_entry{
	struct bitmap *status_map;
	int frame_number;
	// PT ENTRY?
	//struct hash_elem SPT_elem;
};

struct SPT_entry *SPT;

void init_SPT(){
	// allocate sufficient memory
	SPT = malloc(sizeof(SPT_entry));
	// initialize bit field
	bitmap_create (size_t bit_cnt)

	// other stuff
}