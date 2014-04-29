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

#include <immintrin.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>

typedef struct LOCK{
	pthread_mutex_t m;
	int v;
	int count;
} Lock;

inline int lock_init(Lock *lock) {
	pthread_mutex_init(&(lock->m), NULL);
	lock->v = 0;
	lock->count = 0;
}

inline int lock_acquire(Lock *lock) {
	int ret = pthread_mutex_lock(&(lock->m));
	if (ret) lock->v = 1;
	return ret;
}

inline int lock_release(Lock *lock) {
	int ret = pthread_mutex_unlock(&(lock->m));
	if (ret) lock->v = 0;
	return ret;
}

inline int lock_destroy(Lock *lock) {
	lock->v = 0;
	lock->count = 0;
	return  pthread_mutex_destroy(&(lock->m));
}

typedef struct HTM{
	int abort_count;
	int status;
	int retry_count;
	int backoff_value[20];
} Htm;

inline int htm_init(Htm *htm) {
	int i;
	htm->abort_count = 0;
	htm->retry_count = 0;
	for (i = 0; i < 20; i++) {
		htm->backoff_value[i] = pow(2, i);
	}
}

inline int _htm_begin(Htm *htm) {
	htm->status = _xbegin();
	return (htm->status == _XBEGIN_STARTED);
}

inline void _htm_commit() {
	_xend();
}

inline void _htm_abort() {
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
inline int _retry(Htm *htm) {
	// if abort count is large than 10, give up retrying, use fallback path
	if (htm->abort_count >= 10 || htm->status == _XABORT_CAPACITY) {
		return 0;
	}

	// if the abort is because of data conflict, backoff and retry
	if (htm->status == _XABORT_CONFLICT || htm->status == _XABORT_EXPLICIT) {
		return 1;
	}

	if (htm->status == _XABORT_RETRY) {
		return 2;
	}
	// otherwise, keep backoff retrying
	return 1;
}

inline void _backoff(Htm *htm) {
	int count = htm->abort_count % 20, i;
	int backoff_number = htm->backoff_value[count] * 10;
	for (i = 0; i < backoff_number; i++);
}

inline int transaction_start(Htm *htm) {
	for (htm->abort_count = 0; htm->abort_count < 10; htm->abort_count++) {
		//		cout << "abort count: " << abort_count << endl;
		if (_htm_begin(htm)) return 1;		
		else {
			htm->retry_count++;
			int retry_status = _retry(htm);
			if (retry_status == 0) {
				return 0;
			} else if (retry_status == 1) {
				_backoff(htm);
				continue;
			} else if (retry_status == 2) {
				//					cout << "immediately retry " << endl;
				continue;
			}
			else {
				perror("Unexpected Error\n");
				exit(1);
			}
		}
	}
	return 0;
}

inline void transaction_commit() {
	_htm_commit();
}

inline void transaction_abort() {
	_htm_abort();
}


#endif
