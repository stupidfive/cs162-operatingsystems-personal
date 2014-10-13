#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "page.h"
#include "swap.h"

uint32_t extract_tag(uint32_t addr) {
  return addr >> FRAMEBITS;
}

uint32_t extract_offset(uint32_t addr) {
  return addr & FRAMEMASK;
}

/* Page table contains an entry for each physical frame in memory 

   Virtual Address space size = PT_SIZE * FRAMESIZE
 */

pte_t pagetable[PT_SIZE];
int next_pte = 0;
int hits = 0;
int misses = 0;

/* 

Page table operations

 */

void init_pagetable() {
  int i;
  for (i=0; i<PT_SIZE; i++) {	/* initial no valid pages */
    pagetable[i].valid = false;
    pagetable[i].frame = -1;
  }
}

void pte_clean(uint32_t page) {
  pagetable[page].dirty = false;
}

void pte_fresh(uint32_t page, uint32_t frame) {
  pagetable[page].frame = frame;
  pagetable[page].accessed = false;
  pagetable[page].valid = true;
}

void pte_invalid(uint32_t page) {
  pagetable[page].valid = false;
}

bool pte_isdirty(uint32_t page) {
  return pagetable[page].dirty;
}


void print_pagetable() {
  int i;
  for (i = 0; i < PT_SIZE; i++) {
    if (pagetable[i].valid) 
      printf("Page %4d  a: %d d: %d frame: %d\n", i,
	     pagetable[i].accessed,
	     pagetable[i].dirty,
	     pagetable[i].frame);
  }
}



/* 

   Physical memory management
   
   Memory size = MT_SIZE * FRAMESIZE
 */

#define MT_SIZE 4
int resident_page[MT_SIZE];
int next_frame = 0;
bool clock = false;

void init_mem(bool doclock){
  int i;
  for (i=0; i < MT_SIZE; i++) {
    resident_page[i] = -1;
  }
  next_frame = 0;
  clock = doclock;
}

int get_frame() {
  int frm;
  /* Default round robin policy */
  if (!clock) {
    frm = next_frame;
    next_frame = (next_frame + 1) % MT_SIZE;    
    return frm;
  } 
  /* clock algorithm

     Go through the FT looking for PTE that has not been accessed
     clearing the access bits along the way

 */
  // CLOCK ALGORITHM GOES HERE - about 10 lines long
	int frame;
	while (1) {
		if (pagetable[resident_page[next_frame]].accessed) {
			pagetable[resident_page[next_frame]].accessed = false;
		} else {
			frame = next_frame;
			next_frame = (next_frame + 1) % MT_SIZE;
			return frame;
		}
		next_frame = (next_frame + 1) % MT_SIZE;
	}
  return -1;
}

void print_frames() {
  int i;
  printf("Next frame: %d\n", next_frame);
  for (i=0; i<MT_SIZE; i++) printf("Frame: %d is page %d\n", i, resident_page[i]);
}

/* Simulated superduper MMU */

int translate(uint32_t page, uint32_t offset, access_t a) {
  /* map page to PTE
     if valid and match, return frame number holding the page after updating PTE
     otherwise return -1
   */
  uint32_t va = page*FRAMESIZE+offset;
  uint32_t pa;
  uint32_t frm;
  if (pagetable[page].valid) {
    hits++;
    frm = pagetable[page].frame;
    pa = frm*FRAMESIZE+offset;
    pagetable[page].accessed = true;
    if (a == WRITE) pagetable[page].dirty = true;
    printf("ACCESS(%d) %d as %d - page %d in frame %d [%d]\n", a, va, pa, page, frm, offset);
    return frm;
  }
  misses++;
  return -1;
}

/* 

Simulated page fault handler

 */
void page_fault(uint32_t page) {

  // page fault handler goes here - about 10 lines
  // 
	uint32_t frame;
	frame = get_frame();
	if (resident_page[frame] != -1 && pagetable[resident_page[frame]].valid) {
		evict_page(resident_page[frame], frame);
		pte_invalid(resident_page[frame]);
		pte_clean(resident_page[frame]);
		pte_invalid(resident_page[frame]);
	}
	load_page(page, frame);
	resident_page[frame] = page;
  //  print_pagetable();
}

void print_pt_stats() {
  printf("%d hits, %d misses\n", hits, misses);
}
