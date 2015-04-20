//Jason Mar
//902933867
//ECE3056

#ifndef __CACHESIM_H
#define __CACHESIM_H

typedef unsigned long long addr_t;
typedef unsigned long long counter_t;


//added update function to take care of updating stack/set checking for writeback
int update(int remove_index, int value, int write, int set_order);

void cachesim_init(int blocksize, int cachesize, int ways);
void cachesim_access(addr_t physical_add, int write);
void cachesim_print_stats(void);

#endif
