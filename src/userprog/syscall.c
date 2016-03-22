#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  
  //------------------ADDED--------------------------------
  	// a switch statement to differentiate between system calls
  	// intr_frame f has element esp f->esp; arguments for system call
  	// live there*/
  //printf("other hex dump\n");
  //hex_dump((f->esp), (f->esp), 80, 1);

  int syscall_id = *(int*)(f->esp);
  printf ("system call rec'd: ");
  switch(syscall_id){
  	case SYS_HALT:
  		printf("SYS_HALT signal");
  		break;
  	case SYS_EXIT: 
  		printf("SYS_EXIT signal");
  		break;
  	case SYS_EXEC: 
  		printf("SYS_EXEC signal");
  		break;
  	case SYS_WAIT: 
  		printf("SYS_WAIT signal");
  		break;
  	case SYS_CREATE: 
  		printf("SYS_CREATE signal");
  		break;
  	case SYS_REMOVE: 
  		printf("SYS_REMOVE signal");
  		break;
  	case SYS_OPEN: 
  		printf("SYS_OPEN signal");
  		break;
  	case SYS_FILESIZE: 
  		printf("SYS_FILESIZE signal");
  		break;
  	case SYS_READ: 
  		printf("SYS_READ signal");
  		break;
  	case SYS_WRITE:
  		printf("SYS_WRITE signal:\n");
  		int fd = *(int*)(f->esp+4);
  		char ** cp = (char*)(f->esp+8);
  		int char_count = *(int*)(f->esp+12);
		//printf("fd: %d %s : %d \n", fd, *cp, char_count); // fix me
		write(fd, *cp, char_count);
		return;
		// 1 = std out
		//const char * buff = ;
		//write (1, const void *buffer, unsigned size);
  	case SYS_SEEK: 
  		printf("SYS_SEEK signal");
  		break;
  	case SYS_TELL: 
  		printf("SYS_TELL signal");
  		break;
  	case SYS_CLOSE: 
  		printf("SYS_CLOSE signal");
  		break;
  	default:
  		printf("ERROR: uncaught exception");
  }

  //-------------------------------------------------------

  thread_exit ();
}

int write (int fd, const void *buffer, unsigned size){
	if(fd == 1)
		putbuf((char*)buffer, size);
	return 1;
}

/* 0 - 12
	SYS_HALT,                   // Halt the operating system. 
    SYS_EXIT,                   // Terminate this process. 
    SYS_EXEC,                   // Start another process. 
    SYS_WAIT,                   // Wait for a child process to die. 
    SYS_CREATE,                 // Create a file. 
    SYS_REMOVE,                 // Delete a file. 
    SYS_OPEN,                   // Open a file. 
    SYS_FILESIZE,               // Obtain a file's size. 
    SYS_READ,                   // Read from a file. 
    SYS_WRITE,                  // Write to a file. 
    SYS_SEEK,                   // Change position in a file. 
    SYS_TELL,                   // Report current position in a file. 
    SYS_CLOSE,                  // Close a file. 
*/
/*


*/