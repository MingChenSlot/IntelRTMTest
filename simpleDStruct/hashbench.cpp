/**
 * file:sbench.cpp
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
 * This benchmark is used to characterize Intel TSX (restricted transactional memory) with data structures like Array, LinkedList, Queue and ht Table. 
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
#include <unordered_map>
#include <vector>
//#include <algorithm>
#include <list>
#include <set>
#include <time.h>
#include <sched.h>
//#include "stat.h"
#include "synclib.h"
#include "hwtimer.h"

using namespace std;

#define tr(container, it) \
	for(auto it = container.begin(); it != container.end(); it++)

#define SIZE 1000
#define TOTAL_WORKLOAD 1000
#define THREADS 4
#define REPEATS (TOTAL_WORKLOAD / THREADS)

// global data structure
Lock l;
unordered_map<int, int> ht;
unordered_map<int, Lock* > locks;

int access_pattern = 0;
int sync_mode = 0;
int trans_size;

// used to produce specific access pattern
vector<int> v_random;

void bind(int pproc) {
	cpu_set_t myProc;
	CPU_ZERO(&myProc);
	CPU_SET(pproc, &myProc);

	if (sched_setaffinity(0, sizeof(myProc), &myProc) < 0) {
		std::cerr << "Call to sched_setaffinity() failed for physical CPU "
				<< pproc << std::endl;
	}

}


/*************************************
 * Simple Transactional Access (HLE)
 *************************************/
template <class T>
void st_random_access(T &data, long tid, int transac_size, int nit) {
	int base = tid * REPEATS + nit;
//	cout << seed << endl;
	if (_xbegin() == _XBEGIN_STARTED) {
		if (l.is_locked()) {
			_xabort(1);	
		}
		for (int i = 0; i < transac_size; i++) {
			data[v_random[base + i]] = data[v_random[base + i]] + 1;
		}
		_xend();
	} else {
		l.acquire();
		l.add_count();
		for (int i = 0; i < transac_size; i++) {
			data[v_random[base + i]] = data[v_random[base + i]] + 1;
		}
		l.release();
	}
}


/****************************************************
 * Abort Aware Transactional Access
 ****************************************************/
template <class T>
void aat_random_access(T &data, long tid, int transac_size, int nit) {
	int base = tid * REPEATS + nit;
//	cout << seed << endl;
	HTM htm;
	if (htm.transaction_start()) {
		if (l.is_locked()) {
			htm.transaction_abort();	
		}
		for (int i = 0; i < transac_size; i++) {
			data[v_random[base + i]] = data[v_random[base + i]] + 1;
		}
		htm.transaction_commit();	
	} else {
		l.acquire();
		l.add_count();
		for (int i = 0; i < transac_size; i++) {
			data[v_random[base + i]] = data[v_random[base + i]] + 1;
		}
		l.release();
	}
}


/*************************************
 * Coarse-Grained Lock Access
 *************************************/
template <class T>
void gl_random_access(T &data, long tid, int transac_size, int nit) {
	int base = tid * REPEATS + nit;
	l.acquire();
	for (int i = 0; i < transac_size; i++) {
		data[v_random[base + i]] = data[v_random[base + i]] + 1;
	}
	l.release();
}

/*************************************
 * Fine-grained Lock Access
 * ***********************************/
template <class T>
void fl_random_access(T &data, long tid, int transac_size, int nit) {
	int base = tid * REPEATS + nit;
	// sort accessing array entry by the their orders in the array, guaranteeing deadlock free
	set<int> sorted_set;
	list<int> sorted;
	list<int>::iterator it;

	for (int i = 0; i < transac_size; i++) {
		sorted.push_back(v_random[base + i]);
		sorted_set.insert(v_random[base + i]);
	}

	sorted.sort();

	tr (sorted_set, it) {
		locks[*it]->acquire();
	}
	tr (sorted, it) {
		data[*it] = data[*it] + 1;
	}
	tr (sorted_set, it) {
		locks[*it]->release();
	}
}


void *target(void *threadid) {
	long tid = (long) threadid; 
	bind(tid);

	// 2 is simple transaction, 3 is coarse grained lock, 4 is fine-grained lock, 5 is abort code aware transaction
	if (sync_mode == 2) {
		for (int i = 0; i < REPEATS; i++) {
			st_random_access(ht, tid, trans_size, i); 
		}	
	} else if (sync_mode == 3) {
		for (int i = 0; i < REPEATS; i++) {
			gl_random_access(ht, tid, trans_size, i); 
		}	
	} else if (sync_mode == 4) {
		for (int i = 0; i < REPEATS; i++) {
			fl_random_access(ht, tid, trans_size, i); 
		}	
	} else if (sync_mode == 5) {
		for (int i = 0; i < REPEATS; i++) {
			aat_random_access(ht, tid, trans_size, i); 
		}	
	}
	return 0;
}


int main(int argc, char **argv) {
	pthread_t threads[THREADS];
	int seed;	
	
	if (argc != 3) {
		cout << "./hash transactional_size sync_mode(-st -gl -fl -aat) " << endl;
		return 0;
	}
	trans_size = atoi(argv[1]);

	// init data structure
	unordered_map<int, int>::iterator it;
	for (int i = 0; i < (REPEATS * THREADS * trans_size); i++) {
		srand(time(NULL) + i);
		seed = rand() % (SIZE - trans_size);
		v_random.push_back(seed);
		// assign 1 to all bucket in ht table (key, 1)
		ht[seed] = 1;
	}

	cout << "Hash Size: " << ht.size() << endl;

	tr (ht, it)	{
		locks[it->first] = new Lock;
	}

	//shuffle the vector to produce random access workload
	/*
	random_shuffle (v_random.begin(), v_random.end());
	random_shuffle (v_random.begin(), v_random.end());
*/

	// timer init
	hwtimer_t timer;
	initTimer(&timer);
	
	
	if (strcmp(argv[2], "-st") == 0) {
		// htm
		sync_mode = 2;
	} else if(strcmp(argv[2], "-gl") == 0) {
		// global lock used
		sync_mode = 3;
	} else if(strcmp(argv[2], "-fl") == 0) {
		// fine-grained lock used
		sync_mode = 4;
	} else if(strcmp(argv[2], "-aat") == 0) {
		// fine-grained lock used
		sync_mode = 5;
	} else { 
		cout << "./hash transactional_size sync_mode(-st -gl -fl -aat) " << endl;
		return 0;
	}

	startTimer(&timer);
	for (long t = 0; t < THREADS; t++) {
		if (pthread_create(&threads[t], NULL, &target, (void *)t)) { 
			cout << "create thread " << t << " failed\n";
			return -1;
		}
	}

	for (int i = 0; i < THREADS; i++) {
		if (pthread_join(threads[i], NULL)) {
			cout << "join thread " << i << " failed\n";
			return -1;
		}
		cout << "Thread " << i << " joined.\n";
	}
	stopTimer(&timer);
	cout << "Elapsed time for program: " << getTimerNs(&timer) << " ns" << endl;

/*	
	tr (ht, it) {
		cout << it->second << " ";
	}
	cout << endl;
*/
/*
	tr (v_random, it) {
		cout << *it << " ";
	}
	cout << endl;
*/
	tr (locks, it)	{
		delete locks[it->first];
	}
	cout << "Lock count:" << l.get_count() << endl;
	return 0;
}
