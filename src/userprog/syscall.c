#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
// added includes
#include "threads/vaddr.h"
#include "userprog/pagedir.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f )//UNUSED) 
{
  
  //------------------ADDED--------------------------------
  	// a switch statement to differentiate between system calls
  	// intr_frame f has element esp f->esp; arguments for system call
  	// live there*/
  //printf("other hex dump\n");
  //hex_dump((f->esp), (f->esp), 80, 1);

  // validate memory
  printf("esp = %p\n", f->esp);
  if (!is_user_vaddr(f->esp)){
    printf("ptr not virtual address\n");
    //replace with system call
    thread_exit();
  }



  int syscall_id = *(int*)(f->esp);
  printf ("system call rec'd: ");
  switch(syscall_id){
  	case SYS_HALT:
  		printf("SYS_HALT signal\n");
  		break;
  	case SYS_EXIT:
  		printf("SYS_EXIT signal\n");
      exit(666); // fix this
  		break;
  	case SYS_EXEC: 
  		printf("SYS_EXEC signal\n");
  		break;
  	case SYS_WAIT: 
  		printf("SYS_WAIT signal\n");
  		break;
  	case SYS_CREATE: 
  		printf("SYS_CREATE signal\n");
  		break;
  	case SYS_REMOVE: 
  		printf("SYS_REMOVE signal\n");
  		break;
  	case SYS_OPEN: 
      ASSERT(false);
  		printf("SYS_OPEN signal\n");
  		break;
  	case SYS_FILESIZE: 
  		printf("SYS_FILESIZE signal\n");
  		break;
  	case SYS_READ: 
  		printf("SYS_READ signal\n");
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
  		printf("SYS_SEEK signal\n");
  		break;
  	case SYS_TELL: 
  		printf("SYS_TELL signal\n");
  		break;
  	case SYS_CLOSE: 
  		printf("SYS_CLOSE signal\n");
  		break;
  	default:
  		printf("ERROR: uncaught exception");
  }

  //-------------------------------------------------------

  thread_exit ();
}
/*
void halt (void) NO_RETURN;
void exit (int status) NO_RETURN;
pid_t exec (const char *file);
int wait (pid_t);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
*/
void halt (void){
  shutdown_power_off();
}

void exit (int status){
  //set current program's status to new status
  struct thread_child * child_struct_ptr  = list_entry (thread_current()->child_list_elem, struct thread_child, elem);
  child_struct_ptr->exit_status = status;
  printf("exiting with status %d\n", child_struct_ptr->exit_status);
  //then call function to exit it, I think one might exist
  thread_exit(); // this is maybe not right, eh buddeh?
}

pid_t exec (const char *cmd_line){
  return 0; // placeholder
}

int wait (pid_t pid){
  //make sure process wait is implemented
  //get child id? or just pid? And pass into process_wait
  return process_wait(pid);
}

bool create (const char *file, unsigned initial_size){
  //need to copy user file into kernel memory
  //bool success = filesys_create(filecopy, initial_size, i think it's FILE_INODE which is a file sys object);
  //free the filecopy
  //return success;
  return false; // placeholder
}

bool remove (const char *file){
  //need to copy user file into kernel memory
  //bool success = filesys_remove(filecopy)
  //free the filecopy
  //return success;
  return false; // placeholder
}

int open (const char *file){
  return 0; // placeholder
}

int filesize (int fd){
  return 0; // placeholder
}
int read (int fd, void *buffer, unsigned length){
  return 0; // placeholder
}

int write (int fd, const void *buffer, unsigned size){
  if(fd == 1)
    putbuf((char*)buffer, size);
  return 1;
}

/*
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);
*/
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