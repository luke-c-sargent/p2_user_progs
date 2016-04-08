// frame table

struct FrameTableEntry {
	void * frame_ptr;
	int status;
};

struct FrameTableEntry* frame_table;

// function declarations
struct FrameTableEntry* alloc_frame_table();