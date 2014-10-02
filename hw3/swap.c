#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "swap.h"
#include "page.h"

int evictions = 0;
int loads = 0;
int dumps = 0;

/* sleep time in usecs to simulate the feel of going to disk */
#define SLEEPUS 250000
/* 

   Crude simulation of the I/O system for handling page faults

*/

void dump_page(uint32_t page, uint32_t frame) {
  /* simulate dumping a page from memory to disk */
  printf("DUMP page %d from frame %d ..", page, frame); 
  fflush(stdout);
  dumps++;
  usleep(SLEEPUS);  
  printf(".. done\n" );
  pte_clean(page);
}

void load_page(uint32_t page, uint32_t frame) {
  /* simulate loading a page into memory from disk */
  printf("LOAD page %d into frame %d ..", page, frame); 
  fflush(stdout);
  loads++;
  usleep(SLEEPUS);  
  pte_fresh(page, frame);
  printf(".. done\n" );
}

void evict_page(uint32_t page, uint32_t frame) {
  /* simulate eviction of a page from the page table */
  if (pte_isdirty(page)) dump_page(page, frame);
  printf("EVICT page %d from frame %d\n", page, frame);
  evictions++;
}

void print_swap_stats() {
  printf("evictions: %d, loads: %d, dumps: %d\n", evictions, loads, dumps);
}
