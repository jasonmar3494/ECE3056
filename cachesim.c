//Jason Mar
//902933867
//ECE3056

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cachesim.h"

//structure stack for containing LRU stack
typedef struct stack{
	int LRU[512];
	int size;
}stack;

//structure row to define the row containing all ways of cache
typedef struct row{
	int size;
	int valid[512];
	int tag[512];
	int dirty[512];
}row;

//structure cache to contain all rows and LRU stacks
typedef struct cache{
	row* row;
	stack* stack;	
	int rows;
	int block;
	int size;
}cache;

//Global variables
counter_t accesses = 0, hits = 0, misses = 0, writebacks = 0, reads = 0, writes = 0, readmiss = 0, writemiss = 0;

int blockData;
int rowData;
int tagData;

int blockBits;
int rowBits;
int tagBits;

cache* cacheobj;

//added update function to update the cache
int update( int v, int wr, int row_order,int indexToRemove){
	
	row* row = &(cacheobj->row[row_order]);
	stack* stack = &(cacheobj->stack[row_order]);	
	
	//update stack
	int removedFromStack = stack->LRU[indexToRemove];
	for (int i = 0; i < stack->size; i++){
		//shift up
		if (i >= indexToRemove){
			stack->LRU[i] = stack->LRU[i + 1];
		}
	}
	int newsize =stack->size - 1;
	stack->LRU[newsize] = v;

	//updates current row
	for (int i = 0; i < row->size; i++){
		if (row->tag[i] == removedFromStack){
			row->tag[i] = v;
			row->valid[i] = 1;
			if (wr == 1){
				row->dirty[i] = 1;
			}
		}
	}

	//check for writeback
	int writeback = 0;
	int row_index=-1;
	for (int i = 0; i < row->size; i++){
		if (row->tag[i] == removedFromStack){
			row_index = i;
		}
	}
	if (row->dirty[row_index] == 1){
		writeback = 1;
	}
	
	return writeback;
}


// This is where the length of different fields. It also takes in the cache_init function.
void cachesim_init(int blocksize, int cachesize, int ways) {
	int numrows =(cachesize / blocksize / ways);
		
	//find the length in bits of each cache element
	blockBits = log(blocksize)/log(2);
	rowBits = log(cachesize/(blocksize*ways))/log(2);
	tagBits = 32-blockBits-rowBits;
	
	//row cache values
	cacheobj = (cache*)malloc(sizeof(cache));	
	stack* stackobj = (stack*)malloc(sizeof(stack)*numrows);
	row* rowobj = (row*)malloc(sizeof(row)*numrows);
	cacheobj->rows = ways;
	cacheobj->block = blocksize;
	cacheobj->size = cachesize;
	//allocate for rows/stacks
	
	
	//rows each LRU stack to -1
	for (int i = 0; i < numrows; i++){
		stack* stackpoint = &stackobj[i];
		row* rowpoint = &rowobj[i];
		stackpoint->size = ways;
		for (int j = 0; j < ways; j++){
			stackpoint->LRU[j] = -1;
		}
		
		//sets tags to stack val
		//sets dirty/valid bits to 0
	  rowpoint->size = stackpoint->size;
	  for (int k = 0; k < stackpoint->size; k++){
			rowpoint->tag[k] = stackpoint->LRU[k];
			rowpoint->dirty[k] = 0;			
			rowpoint->valid[k] = 0;
			cacheobj->row = rowobj;
			cacheobj->stack = stackobj;
		}	
	}
}


//method for extracting physical data and counting hits/misses/writebacks
void cachesim_access(addr_t physical_addr, int write) {
	if(write == 1){
		writes++;
	}
	else{
		reads++;
	}
	//get values from physical addr
	//mask out block/row/tag using shifts 1000 - 1 => 0111
	blockData = physical_addr & ((1 << blockBits) - 1);			
	rowData = (physical_addr >> blockBits) & ((1 << rowBits) - 1);
	tagData = (physical_addr >> (blockBits + rowBits)) & ((1 << tagBits) - 1);
	
	stack* stackobj = &(cacheobj->stack[rowData]);
	row* rowobj = &(cacheobj->row[rowData]);

	int hit = -1;
	//check LRU
	for (int i = 0; i < stackobj->size; i++){
		if (stackobj->LRU[i] == tagData){
			hit = i;
		}
	}
	
	//if there is a hit, update the cache
	if (hit !=-1){
		update( tagData, write, rowData,hit);
		hits++;
	}	
	 
	//if no hit, update cache and writeback if needed
	else if (hit==-1){
		if (update(tagData, write, rowData,0)){
			
			printf("%llu", writebacks);
		}
		if(write == 1){
			writemiss++;
		}
		else{
			readmiss++;
	}
		misses++;
	}
	//increment accesses
	accesses++;
}

void cachesim_print_stats() {
	printf("read misses: %llu\t write misses: %llu\n", readmiss, writemiss);
	printf("reads: %llu\t\t writes: %llu\n", reads, writes);
		
	printf("accesses: %llu\t hits: %llu\t misses: %llu\t writebacks: %llu\n", accesses, hits, misses, writebacks);
}
