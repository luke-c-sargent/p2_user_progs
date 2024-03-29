#include "userprog/exception.h"
#include <inttypes.h>
#include <stdio.h>
#include "userprog/gdt.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
//--------------
#include <stdlib.h>
#include "userprog/syscall.h"
#include "threads/vaddr.h"
#include "vm/page.h"
#include "userprog/process.h"
#include "filesys/file.h"
#include "vm/swap.h"

#define SYSCALL_ERROR -1
#define DEBUG 0
//--------------------

/* Number of page faults processed. */
static long long page_fault_cnt;

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
    if (DEBUG)
      printf ("PAGE FAULT: fault address was NULL\n");
    exit(SYSCALL_ERROR);
  }
  //=========================================

  /* Count page faults. */
  page_fault_cnt++;

  /* Determine cause. */
  not_present = (f->error_code & PF_P) == 0;
  write = (f->error_code & PF_W) != 0;
  user = (f->error_code & PF_U) != 0;

  struct thread* t = thread_current ();
  if (DEBUG) {
    printf ("thread esp: %p\nstack_pointer:%p \nf->esp:%p\n", t->esp, t->stack_pointer, f->esp);
    printf ("PAGE FAULT: fault addr: %p\n", fault_addr);
  }

  void* page_start = pg_round_down (fault_addr);
  struct SPT_entry* spte = get_SPT_entry (page_start);

  if (spte)
  {
    if (false && DEBUG && is_user_vaddr(fault_addr))
    {
      printf ("~~SPT: sp:%d\tvaddr:%p\tpg#:%d\trb:%d\n\tofs:%d\trbyte:%d\tzbyte:%d\twrt:%d\n", 
        spte->is_stack_page, spte->vaddr, spte->page_number, spte->resident_bit, spte->ofs, 
        spte->page_read_bytes, spte->page_zero_bytes, spte->writable);
    }
    if (DEBUG){
      printf ("\tthread %s fault address %p rounded to %p \n\n", thread_current ()->name, fault_addr, page_start);
    }

    /* Get a page of memory. */
    uint8_t *kpage = get_user_page(page_start);
    if (kpage == NULL){
      printf ("KPAGE ALLOCATION FAIL\n"); // eviction
      exit(SYSCALL_ERROR);
    }

    // swap stuff 
    if(!spte->resident_bit && (spte->dirty_bit || spte->is_stack_page)){
      if(DEBUG)
        printf("Resident bit: %d, Dirty bit: %d, is stack page: %d\n", spte->resident_bit, spte->dirty_bit, spte->is_stack_page);
      remove_swap_entry(spte->swap_index, kpage, spte);
    }
    else
    {

      file_seek (t->executable, spte->ofs);

      /* Load this page. */
      if (file_read (t->executable, kpage, spte->page_read_bytes) != (int) spte->page_read_bytes)
          {
            if (DEBUG)
              printf ("\tfile read FAILURE :C :C :C :C\n");
            //palloc_free_page (kpage);
            //return false; 
          }
      memset (kpage + spte->page_read_bytes, 0, spte->page_zero_bytes);
    }

    /* Add the page to the process's address space. */
    if (!install_page (page_start, kpage, spte->writable)) 
     {
      if (DEBUG){
        printf ("kpage %p\n", kpage);
        //palloc_free_page (kpage); // dis aint good
        printf ("ERROR IN INSTALL PAGE\n");}
       //return false; 
       exit(SYSCALL_ERROR);
     }
    spte->resident_bit = true;
  } else 
  {
    if (DEBUG)
      printf ("PAGE FAULT: could not find SPT entry\n");
      // stack growth logic --------------------------------------------
    if (DEBUG) {
      printf ("subtracting f->esp %p and fault_addr %p = %p \n", f->esp, fault_addr, (void*)((uint32_t)f->esp - (uint32_t)fault_addr));
      printf ("subtracting t->esp %p and fault_addr %p = %p \n", t->esp, fault_addr, t->esp - fault_addr);
    }
    if (abs ((uint32_t) f->esp - (uint32_t) fault_addr)<= 32 || t->esp > f->esp || (!t->esp && fault_addr > f->esp) || (t->esp && t->esp < fault_addr))
    {
      if (DEBUG)
        printf ("trying to grow stack\n");
      // get a page, add a SPT entry, install page

      //added-------------------------kernel to kernel
      if (t->esp > f->esp)
      {
        if (DEBUG)
          printf ("Changed page_start to t->esp\n");
        page_start = pg_round_down (t->esp);
      }// end added 
      spte = create_SPT_entry (page_start, true, (off_t)NULL, (size_t)NULL, (size_t)NULL, true);
      uint8_t *kpage = get_user_page (page_start);

      if (!install_page (page_start, kpage, spte->writable))
      {
        if (DEBUG)
          printf ("install page is borked\n");
        exit (SYSCALL_ERROR);
      }
    }
    else
    {
      exit (SYSCALL_ERROR);
    }

  if (DEBUG)
    printf ("\tPage_start: %p\n", page_start);
  } 
}
