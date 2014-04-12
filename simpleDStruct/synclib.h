/**
* file:synclib.h
* @author: Ming Chen 9068207811 <mchen67@wisc.edu>
* @version 1.0
* @created time: Sat 05 Apr 2014 12:39:23 PM CDT
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

#ifndef SYNCLIB_H
#define SYNCLIB_H


#include <mutex>
#include "stat.h"

using namespace std;

// arguments for pthread create
typedef struct Arguments{
	long tid;
	int access_pattern;
	int data_mode;
	int trans_size;
} Args;


// fall back lock
class Lock {
	mutex m;
	bool v;
	int count;

public:
	Lock() { 
		v = false; 
		count = 0;
	}
	~Lock(){
		clear_count();
	}

	void acquire() {	
		m.lock();
		v = true;
	}

	void release() {
		m.unlock();
		v = false;
	}

	bool is_locked() {
		return v;
	}

	void add_count() {
		count++;
	}

	int get_count() {
		return count;
	}

	void clear_count() {
		count = 0;
	}
};


class HTM {
	int abort_count;
	int status;
	int retry_count;
	
	int backoff_value[20] = { 2, 4, 8, 16, 32, 64, 256, 512, 1024, 2048, 3072, 4096, 8192, 16384, 32768, 65536, 131072, 524288, 1048576, 2097152 };

	Statistic stat;
	
	bool htm_begin() {
		status = _xbegin();
//		stat.add_data(status);	
		return (status == _XBEGIN_STARTED);
	}

	void htm_commit() {
		_xend();
	}

	void htm_abort() {
		_xabort(0xff);
	}

	/**
	 * retry method
	 * Only take capacity abort and conflict abort into account, other situations are too
	 * complicated for our course proejct; simple retry method is applied here
	 *
	 * return code:
	 *	0 ~ give up retrying, use fallback path
	 *	1 ~ backoff and retry
	 *	2 ~ immediately retry
	 */
	int retry() {
		// if abort count is large than 10, give up retrying, use fallback path
		if (abort_count >= 10 || status == _XABORT_CAPACITY) {
			return 0;
		}
		
		// if the abort is because of data conflict, backoff and retry
		if (status == _XABORT_CONFLICT || status == _XABORT_EXPLICIT) {
			return 1;
		}
		
		if (status == _XABORT_RETRY) {
			return 2;
		}
		// otherwise, keep backoff retrying
		return 1;
	}

	void backoff() {
		int count = abort_count % 20;
		int backoff_number = backoff_value[count] * 10;
//		cout << "back off " << count  << endl;
		for (int i = 0; i < backoff_number; i++);
	}

public:
	HTM() {
		abort_count = 0;
		retry_count = 0;
	}
	~HTM() {}

	int transaction_start() {
		for (abort_count = 0; abort_count < 10; abort_count++) {
	//		cout << "abort count: " << abort_count << endl;
			if (htm_begin()) return 1;		
			else {
				retry_count++;
				int retry_status = retry();
				if (retry_status == 0) {
					return 0;
				} else if (retry_status == 1) {
					backoff();
					continue;
				} else if (retry_status == 2) {
//					cout << "immediately retry " << endl;
					continue;
				}
				else {
					cerr << "Unexpected Error" << endl;
					exit(1);
				}
			}
		}
		return 0;
	}

	void transaction_commit() {
		htm_commit();
	}

	void transaction_abort() {
		htm_abort();
	}
	
	void print_retrycount() {
		cout << "Retry Count: " << retry_count << endl;	
	}

	void print_stat() {
		stat.print_stat(0);
	}
};

#endif
