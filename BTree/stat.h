/**
* file:stat.h
* @author: Ming Chen 9068207811 <mchen67@wisc.edu>
* @version 1.0
* @created time: Mon 21 Oct 2013 11:42:44 PM CDT
* 
* @section LICENSE
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
* 
* @section DESCRIPTION
* 
*/

#define NUM_ABORTS 8

typedef struct STAT{
	int counter[NUM_ABORTS];
} Stat;

inline void init_stat(Stat *stat) {
	int i = 0;
	for (; i < NUM_ABORTS; ++i) {
		stat->counter[i] = 0;
	}
}
	
inline void add_stat(Stat *stat, int status) {
	if (status & _XBEGIN_STARTED) {
		stat->counter[0]++;
	} else if (status & _XABORT_EXPLICIT) {
		stat->counter[1]++;
	} else if (status & _XABORT_RETRY) {
		stat->counter[2]++;
	} else if (status & _XABORT_CONFLICT) {
		stat->counter[3]++;
	} else if (status & _XABORT_CAPACITY) {
		stat->counter[4]++;
	} else if (status & _XABORT_DEBUG) {
		stat->counter[5]++;
	} else if (status & _XABORT_NESTED) {
		stat->counter[6]++;
	} else {
		stat->counter[7]++;
	}
}
	
inline void print_stat(Stat *stat) {

	printf("\tSucc\t");
	printf("Exp\t");
	printf("Retry\t");
	printf("Confl\t");
	printf("Cap\t");
	printf("Debug\t");
	printf("Nest\t");
	printf("Other\t\n");
	int i = 0;

	for (; i < NUM_ABORTS; ++i) {
		printf("\t%d", stat->counter[i]);
	}
	printf("\n");
	init_stat(stat);
}

