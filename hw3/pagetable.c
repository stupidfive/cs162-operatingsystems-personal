#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "page.h"
#include "swap.h"

/* 
   simulated processor-memory interface 
   
   This represents a single access to the virtual address space
   It presents an address and a mode in order to go through the address translation process
   The value is ignored, as we are not trying to run programs.

 */
void doaccess(uint32_t addr, access_t mode) {
  uint32_t page = extract_tag(addr);
  uint32_t offset = extract_offset(addr);
  int frame;
  if (addr > MAXADDR) {
    printf("Bad address: %X\n", addr);
  } else {
    frame = translate(page, offset, mode); /* attempt to translate VA to PA */
    if (frame < 0) {		/* page fault */
      page_fault(page);		/* handle the page fauls  */
      doaccess(addr, mode);	/* try again after loading page */
    }
  }
}

/* 
   Access pattern generators
 */

void lineartest() {
  int addr;
  for (addr = 0; addr < FRAMESIZE * PT_SIZE; addr = addr + FRAMESIZE/3) {
    doaccess(addr, READ);
  }
}

void triangletest() {
  int addr, maxaddr;
  for (maxaddr = 0; maxaddr < (FRAMESIZE * PT_SIZE); maxaddr = maxaddr + FRAMESIZE) {
    for (addr = 0; addr < maxaddr; addr = addr + FRAMESIZE/3) {
      doaccess(addr, READ);
    }
  }
}

void triangletestRW() {
  int addr, maxaddr;
  access_t modepattern[8] = {READ, READ, READ, READ, READ, READ, WRITE, READ};
  int mi = 0;
  for (maxaddr = 0; maxaddr < (FRAMESIZE * PT_SIZE); maxaddr = maxaddr + FRAMESIZE) {
    for (addr = 0; addr < maxaddr; addr = addr + FRAMESIZE/3) {
      doaccess(addr, modepattern[mi%8]);
      mi++;
    }
  }
}

void triangletestloc() {
  int addr, maxaddr;
  access_t modepattern[8] = {READ, READ, READ, READ, READ, READ, WRITE, READ};
  int mi = 0;
  for (maxaddr = FRAMESIZE-1; maxaddr < (FRAMESIZE * PT_SIZE); maxaddr = maxaddr + FRAMESIZE) {
    for (addr = 0; addr < maxaddr; addr = addr + FRAMESIZE/3) {
      doaccess(addr, modepattern[++mi%8]);
      doaccess(maxaddr-1, modepattern[++mi%8]);
    }
  }
}

/* 
   Run a pattern generator through a simulation of virtual address translation throgh a pagetable
 */

int main (int argc, char *argv[]) {
  init_pagetable();
  if (argc > 2) init_mem(true);
  else init_mem(false);
  //  print_frames(); 
  if ((argc > 1) && (atoi(argv[1]) == 3)) triangletestloc();
  else if ((argc > 1) && (atoi(argv[1]) == 2)) triangletestRW();
  else if ((argc > 1) && (atoi(argv[1]) == 1)) triangletest();
  else lineartest();;

  //  print_pagetable();
  //  print_frames();
  printf("PTE size: %ld bytes\n",sizeof(pte_t));
  print_pt_stats();
  print_swap_stats();
  return 0;
}

