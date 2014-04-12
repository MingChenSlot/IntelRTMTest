/**
 * file:multi_t_bench.cpp
 * @author: Ming Chen 9068207811 <mchen67@wisc.edu>
 * @version 1.0
 * @created time: Mon 21 Oct 2013 10:19:47 PM CDT
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

#include <iostream>
#include <immintrin.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <iomanip>
#include <pthread.h>
#include <vector>
#include <sched.h>
#include "stat.h"

using namespace std;

#define IDX 2
#define NUM_REPEAT 100
#define ARRAY_SIZE (IDX * 8192)
#define STEP (IDX * 4096)

void bind(int pproc) {
	cpu_set_t myProc;
	CPU_ZERO(&myProc);
	CPU_SET(pproc, &myProc);

	if (sched_setaffinity(0, sizeof(myProc), &myProc) < 0) {
		std::cerr << "Call to sched_setaffinity() failed for physical CPU "
				<< pproc << std::endl;
	}

}

size_t arr[ARRAY_SIZE];

void *target(void *threadid) {

	long tid = (long) threadid;
	bind(tid);

	Statistic stat;	
	
	long sig;
	// sig = tid;
	sig = 0;

	for (int repeat = 0; repeat < NUM_REPEAT; repeat++) {	
		if (tid == 0) {
			int status;	
			if ((status = _xbegin()) == _XBEGIN_STARTED) {
				for (int i = sig * STEP; i < STEP * (sig + 2); i++) {
				//	arr[i] = i;
					arr[i];
				}
				_xend();
			}
			stat.add_data(status);
		} else {
			for (int i = (sig + 2) * STEP; i < STEP * (sig); i--)
				arr[i] = i;
		}
	}

	if (tid == 1)
		sleep(2);
	cout << "THREAD " << tid << endl;
	if (tid == 0) {
		stat.print_stat(ARRAY_SIZE);
	}
	return 0;
}


int main() {
	pthread_t threads[2];

	for (int i = 0; i < ARRAY_SIZE; i++)
		arr[i] = i;

	for (long t = 0; t < 2; t++) {
		if (pthread_create(&threads[t], NULL, &target, (void *)t)) {
			cout << "create thread " << t << " failed\n";
			return -1;
		}
	}

	for (int i = 0; i < 2; i++) {
		if (pthread_join(threads[i], NULL)) {
			cout << "join thread " << i << " failed\n";
			return -1;
		}
		cout << "Thread " << i << " joined.\n";
	}

	return 0;

}
