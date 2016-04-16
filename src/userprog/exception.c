#include "userprog/exception.h"
#include <inttypes.h>
#include <stdio.h>
#include "userprog/gdt.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
//--------------
#include "userprog/syscall.h"
#include "threads/vaddr.h"
#include "vm/page.h"
#include "userprog/process.h"

#define SYSCALL_ERROR -1
#define DEBUG 0

//--------------------

/* Number of page faults processed. */
static long long page_fault_cnt;
uint32_t max_stack = 1024*1024*8; //8MBs
static void kill (struct intr_frame *);
static void page_fault (struct intr_frame *);

/* Registers handlers for interrupts that can be caused by user
   programs.

   In a real Unix-like OS, most of these interrupts would be
   passed along to the user process in the form of signals, as
   described in [SV-386] 3-24 and 3-25, but we don't implement
   signals.  Instead, we'll make them simply kill the user
   process.

   Page faults are an exception.  Here they are treated the same
   way as other exceptions, but this will need to change to
   implement virtual memory.

   Refer to [IA32-v3a] section 5.15 "Exception and Interrupt
   Reference" for a description of each of these exceptions. */
void
exception_init (void) 
{
  /* These exceptions can be raised explicitly by a user program,
     e.g. via the INT, INT3, INTO, and BOUND instructions.  Thus,
     we set DPL==3, meaning that user programs are allowed to
     invoke them via these instructions. */
  intr_register_int (3, 3, INTR_ON, kill, "#BP Breakpoint Exception");
  intr_register_int (4, 3, INTR_ON, kill, "#OF Overflow Exception");
  intr_register_int (5, 3, INTR_ON, kill,
                     "#BR BOUND Range Exceeded Exception");

  /* These exceptions have DPL==0, preventing user processes from
     invoking them via the INT instruction.  They can still be
     caused indirectly, e.g. #DE can be caused by dividing by
     0.  */
  intr_register_int (0, 0, INTR_ON, kill, "#DE Divide Error");
  intr_register_int (1, 0, INTR_ON, kill, "#DB Debug Exception");
  intr_register_int (6, 0, INTR_ON, kill, "#UD Invalid Opcode Exception");
  intr_register_int (7, 0, INTR_ON, kill,
                     "#NM Device Not Available Exception");
  intr_register_int (11, 0, INTR_ON, kill, "#NP Segment Not Present");
  intr_register_int (12, 0, INTR_ON, kill, "#SS Stack Fault Exception");
  intr_register_int (13, 0, INTR_ON, kill, "#GP General Protection Exception");
  intr_register_int (16, 0, INTR_ON, kill, "#MF x87 FPU Floating-Point Error");
  intr_register_int (19, 0, INTR_ON, kill,
                     "#XF SIMD Floating-Point Exception");

  /* Most exceptions can be handled with interrupts turned on.
     We need to disable interrupts for page faults because the
     fault address is stored in CR2 and needs to be preserved. */
  intr_register_int (14, 0, INTR_OFF, page_fault, "#PF Page-Fault Exception");
}

/* Prints exception statistics. */
void
exception_print_stats (void) 
{
  printf ("Exception: %lld page faults\n", page_fault_cnt);
}

/* Handler for an exception (probably) caused by a user process. */
static void
kill (struct intr_frame *f) 
{
  /* This interrupt is one (probably) caused by a user process.
     For example, the process might have tried to access unmapped
     virtual memory (a page fault).  For now, we simply kill the
     user process.  Later, we'll want to handle page faults in
     the kernel.  Real Unix-like operating systems pass most
     exceptions back to the process via signals, but we don't
     implement them. */
     
  /* The interrupt frame's code segment value tells us where the
     exception originated. */
  switch (f->cs)
    {
    case SEL_UCSEG:
      /* User's code segment, so it's a user exception, as we
         expected.  Kill the user process.  */
      printf ("%s: dying due to interrupt %#04x (%s).\n",
              thread_name (), f->vec_no, intr_name (f->vec_no));
      intr_dump_frame (f);
      exit(SYSCALL_ERROR);
      //thread_exit (); 

    case SEL_KCSEG:
      /* Kernel's code segment, which indicates a kernel bug.
         Kernel code shouldn't throw exceptions.  (Page faults
         may cause kernel exceptions--but they shouldn't arrive
         here.)  Panic the kernel to make the point.  */
      intr_dump_frame (f);
      PANIC ("Kernel bug - unexpected interrupt in kernel"); 

    default:
      /* Some other code segment?  Shouldn't happen.  Panic the
         kernel. */
      printf ("Interrupt %#04x (%s) in unknown segment %04x\n",
             f->vec_no, intr_name (f->vec_no), f->cs);
      thread_exit ();
    }
}

/* Page fault handler.  This is a skeleton that must be filled in
   to implement virtual memory.  Some solutions to project 2 may
   also require modifying this code.

   At entry, the address that faulted is in CR2 (Control Register
   2) and information about the fault, formatted as described in
   the PF_* macros in exception.h, is in F's error_code member.  The
   example code here shows how to parse that information.  You
   can find more information about both of these in the
   description of "Interrupt 14--Page Fault Exception (#PF)" in
   [IA32-v3a] section 5.15 "Exception and Interrupt Reference". */
static void
page_fault (struct intr_frame *f) 
{
  bool not_present;  /* True: not-present page, false: writing r/o page. */
  bool write;        /* True: access was write, false: access was read. */
  bool user;         /* True: access by user, false: access by kernel. */
  void *fault_addr;  /* Fault address. */

  /* Obtain faulting address, the virtual address that was
     accessed to cause the fault.  It may point to code or to
     data.  It is not necessarily the address of the instruction
     that caused the fault (that's f->eip).
     See [IA32-v2a] "MOV--Move to/from Control Registers" and
     [IA32-v3a] 5.15 "Interrupt 14--Page Fault Exception
     (#PF)". */
  asm ("movl %%cr2, %0" : "=r" (fault_addr));

  /* Turn interrupts back on (they were only off so that we could
     be assured of reading CR2 before it changed). */
  intr_enable ();

  //=========================================
  if (fault_addr == NULL)
  {
    if(DEBUG)
      printf("PAGE FAULT: fault address was NULL\n");
    exit(SYSCALL_ERROR);
  }
  //=========================================

  /* Count page faults. */
  page_fault_cnt++;

  /* Determine cause. */
  not_present = (f->error_code & PF_P) == 0;
  write = (f->error_code & PF_W) != 0;
  user = (f->error_code & PF_U) != 0;

  /* To implement virtual memory, delete the rest of the function
     body, and replace it with code that brings in the page to
     which fault_addr refers. */

  // --------------------------------------------------

  /* Ali: what to do
    1) Find the STE associated to fault_addr

    2) access the file and load PGSIZE of data starting from page start

    3) Memset a page of the stack page, but only a page. Then the
    next time we page fault, memset another page of the stack,
    and so on. This is what growing the stack is and it requires
    keeping track of our position in the loading file as we increment
    that pointer by PGSIZE

    4) Update necessary parts of memory???
  */

  struct thread* t = thread_current();
  if(DEBUG) {
    printf("thread esp: %p\nstack_pointer:%p \nf->esp:%p\n", t->esp, t->stack_pointer, f->esp);
    printf("PAGE FAULT: fault addr: %p\n", fault_addr);
  }

  void* page_start = pg_round_down (fault_addr);
  struct SPT_entry* spte = get_SPT_entry(page_start);


  //void* paddr = pagedir_get_page (&t->pagedir, fault_addr);  //Ali: &fault_addr?

  //if(DEBUG && paddr == NULL)
    //printf("Page Fault: VA is not mapped\n");
  

  

  if(spte){
    if(DEBUG && is_user_vaddr(fault_addr)){
      printf("~~SPT: sp:%d\tvaddr:%p\tpg#:%d\trb:%d\n\tofs:%d\trbyte:%d\tzbyte:%d\twrt:%d\n", spte->is_stack_page, spte->vaddr, spte->page_number, spte->resident_bit, spte->ofs, spte->page_read_bytes, spte->page_zero_bytes, spte->writable);
      printf("\tthread %s fault address %p rounded to %p \n\n", thread_current()->name, fault_addr, page_start);
    }

    file_seek (t->executable, spte->ofs);
    /* Get a page of memory. */
    uint8_t *kpage = get_user_page(page_start);

    if (kpage == NULL){
      printf("KPAGE ALLOCATION FAIL\n"); // eviction
    }


    /* Load this page. */
    if (file_read (t->executable, kpage, spte->page_read_bytes) != (int) spte->page_read_bytes)
        {
          if(DEBUG){printf("\tfile read FAILURE :C :C :C :C\n");}
          palloc_free_page (kpage);
          return false; 
        }
      memset (kpage + spte->page_read_bytes, 0, spte->page_zero_bytes);

    /* Add the page to the process's address space. */
    if (!install_page (page_start, kpage, spte->writable)) 
     {
       palloc_free_page (kpage); // dis aint good
       printf("ERROR IN INSTALL PAGE\n");
       exit(SYSCALL_ERROR);
       //return false; 
     }
    spte->resident_bit = true;
  } else { // NULL
    if(DEBUG)
      printf("PAGE FAULT: could not find SPT entry\n");
      // stack growth logic --------------------------------------------
    //void * limit = PHYS_BASE - PGSIZE - 0x20;
    void* stack_boundary = PHYS_BASE -max_stack;
    if(DEBUG) {
      printf("subtracting %p and %p = %p \n", f->esp, fault_addr, f->esp - fault_addr);
      printf("subtracting %p and %p = %p \n", t->esp, fault_addr, t->esp - fault_addr);
      printf("stack_boundary: %p\n", stack_boundary);
    }
    if(((uint32_t) f->esp - (uint32_t) fault_addr)<=32  || t->esp >= stack_boundary){
      if(DEBUG)
        printf("trying to grow stack\n");
      // get a page, add a SPT entry, install page

      spte = create_SPT_entry(page_start, true, NULL, NULL, NULL, true);
      uint8_t *kpage = get_user_page(page_start);
      if (!install_page (page_start, kpage, spte->writable))
        printf("install page is borked\n");
    }/* else if (( (uint32_t) t->esp - (uint32_t) 2)<=32) {
      if(DEBUG)
        printf("kernel to kernel BIIIIIIIIIITCH\n");
    } */ else{
      if(DEBUG)
        printf("ffaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaack\n");
      exit(SYSCALL_ERROR);
    }
  //------------------------------------------------------------
    
  }
  //t->esp = NULL;

  // --------------------------------------------------
  if(DEBUG)
    printf("\tPage_start: %p\n", page_start);

  // printf ("Page fault at %p: %s error %s page in %s context.\n\n",
  //         fault_addr,
  //         not_present ? "not present" : "rights violation",
  //         write ? "writing" : "reading",
  //         user ? "user" : "kernel");

  //printf("There is like totally a lot of crying in Pintos!\n");
  //printf(" :C :C :C :C :C :C :C :C :C :C :C :C :C :C :C :C\n");
  //kill (f);
}
