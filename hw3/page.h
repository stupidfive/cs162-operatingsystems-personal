#ifndef _page_H_
#define _page_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum access_type {READ=0,  WRITE=2,  FETCH=1} access_t;

#define FRAMESIZE 256
#define FRAMEBITS 8
#define FRAMEMASK 0xFF
#define PT_SIZE 8
#define MAXADDR PT_SIZE*FRAMESIZE

uint32_t extract_tag(uint32_t addr);
uint32_t extract_offset(uint32_t addr);

#define BITS
typedef struct pte {
#ifndef BITS
  bool valid;
  bool accessed;
  bool dirty;
  uint32_t frame;
#else
  //BIT PACKED VERSION GOES HERE - only a few chars different from above
	bool valid: 1;
	bool accessed: 1;
	bool dirty: 1;
	uint32_t frame: 20;
#endif
} pte_t;

/*  Page table operations  */

void init_pagetable();
void pte_clean(uint32_t page);
void pte_fresh(uint32_t page, uint32_t frame);
void pte_invalid(uint32_t page);
bool pte_isdirty(uint32_t page);

void print_pagetable();

/*    Physical memory management */
   
#define MT_SIZE 4

void init_mem(bool doclock);
int get_frame();
void print_frames();

/* Simulated superduper MMU */

int translate(uint32_t page, uint32_t offset, access_t a);

/*  Simulated page fault handler  */
void page_fault(uint32_t page);

void print_pt_stats();
#endif
