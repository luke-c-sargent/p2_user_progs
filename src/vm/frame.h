#include "threads/synch.h"
// frame table
enum FT_STATUS {
	FT_EMPTY = 0,
	FT_FULL
};


struct FrameTableEntry {
	void * frame_ptr;
	int status;
};

struct FrameTableEntry* frame_table;
struct semaphore FT_sema;

// function declarations
struct FrameTableEntry* alloc_frame_table();