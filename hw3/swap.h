#ifndef _swap_H_
#define _swap_H_

#include <stdbool.h>
#include <stdint.h>

void dump_page(uint32_t page, uint32_t frame);

void load_page(uint32_t page, uint32_t frame);

void evict_page(uint32_t page, uint32_t frame);

void print_swap_stats();

#endif
