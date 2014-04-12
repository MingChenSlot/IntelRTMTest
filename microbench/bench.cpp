#include <iostream>
#include <immintrin.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sched.h>
#include "hwtimer.h"
#include "stat.h"

using namespace std;

#define tr(c, i) for(typeof(c).begin() i = (c).begin(); i!=(c).end(); i++)
#define NUM_REPEAT 100
#define SZ sizeof(size_t)
#define LEN 2048

size_t test_size[] = { 2, 4, 8, 16, 32, 64, 256, 512, 1024, 2048, 3072, 4096, 8192, 16384, 32768, 65536, 131072, 524288, 1048576, 2097152 };

void bind(int pproc) {
	cpu_set_t myProc;
	CPU_ZERO(&myProc);
	CPU_SET(pproc, &myProc);

	if (sched_setaffinity(0, sizeof(myProc), &myProc) < 0) {
		std::cerr << "Call to sched_setaffinity() failed for physical CPU "
				<< pproc << std::endl;
	}

}

int read(size_t *arr, size_t len, size_t stride) {
	int status;
	if ((status = _xbegin()) == _XBEGIN_STARTED) {
		for (size_t i = 0; i < len; i += stride) {
			arr[i];
		}
		_xend();
	}
	return status;
}

int write(size_t *arr, size_t len, size_t stride) {
	int status;
	if ((status = _xbegin()) == _XBEGIN_STARTED) {
		for (size_t i = 0; i < len; i += stride) {
			arr[i] = 10;
		}
		_xend();
	}
	return status;
}

int rw(size_t *arr, size_t len, size_t stride) {
	int status;
	if ((status = _xbegin()) == _XBEGIN_STARTED) {
		for (size_t i = 0; i < len; i += stride) {
			arr[i] = arr[i - 1] + 10;
		}
		_xend();
	} else {
		// fall back path
	}
	return status;
}

int syscall(size_t *arr, size_t len) {
//	cout << "Syscall test\n";
	int status;
	if ((status = _xbegin()) == _XBEGIN_STARTED) {
		pid_t pid;
		pid = fork();
		_xend();
	}
	return status;
}

int io(size_t *arr, size_t len) {
//	cout << "IO test\n";
	int status;
	if ((status = _xbegin()) == _XBEGIN_STARTED) {
		for (size_t i = 0; i < len; i++) {
			printf("value of array entry:%d", arr[i]);
		}
		_xend();
	}
	return status;
}

void callee(size_t *arr, size_t len) {

	for (size_t i = 0; i < len; i += 1) {
//		arr[i] = i;
		arr[i];
	}

//	arr[2] = 2;
	return;	
}

int pcall(size_t *arr, size_t len) {
//	cout << "Procedure call test\n";
	int status;
	if ((status = _xbegin()) == _XBEGIN_STARTED) {
		callee(arr, len);
		_xend();
	}
	return status;
}

void helper(size_t stride, int mode) {

	int length = sizeof(test_size) / SZ;
	Statistic stat;		

	for (int aidx = 0; aidx < length; aidx++) {
		size_t alen = test_size[aidx];
		if (alen < stride + 1)
			continue;
		
		//init array
		size_t *arr = new size_t[alen];
		for (size_t i = 0; i < alen - 1; ++i) {
			arr[i] = 1;
		}

		for (int repeat = 0; repeat < NUM_REPEAT; repeat++) {
			//access array
			int status;
			switch(mode) {
			case 1: status = read(arr, alen, stride); break;
			case 2: status = write(arr, alen, stride); break;
			case 3: status = rw(arr, alen, stride); break;
			case 4: status = syscall(arr, alen); break;
			case 5: status = io(arr, alen); break;
			case 6: status = pcall(arr, alen); break;
			default: cout << "Error" << endl;
			}
			stat.add_data(status);	
		}
		if (mode == 6) {
			cout << "Element of array:" << arr[2] << endl;
		}
		delete[] arr;
		int arr_size = alen * SZ;	
		stat.print_stat(arr_size);
	}
}

inline void overheadTest(size_t *arr) {
	for (size_t i = 0; i < LEN; i += 1) {
		arr[i] = i;
	}
}

void overheadEstimation() {
	hwtimer_t timer;
	initTimer(&timer);
	int status;
	size_t *arr = new size_t[LEN];


	startTimer(&timer);
	overheadTest(arr);	
	stopTimer(&timer);
	cout << "Cycles for original program: " << getTimerTicks(&timer) << endl;
	
	resetTimer(&timer);
	startTimer(&timer);
	if ((status = _xbegin()) == _XBEGIN_STARTED) {	
		overheadTest(arr);	
		_xend();
	}
	stopTimer(&timer);
	if (status == _XBEGIN_STARTED) cout << "Cycles for xend program: " << getTimerTicks(&timer) << endl;

	resetTimer(&timer);
	startTimer(&timer);
	if ((status = _xbegin()) == _XBEGIN_STARTED) {	
		overheadTest(arr);	
		_xabort(0x00);
	}
	stopTimer(&timer);
	cout << "Cycles for xabort program: " << getTimerTicks(&timer) << endl;
	delete arr;

	return;
}



int main(int argc, char **argv) {
	bind(1);
	int mode;
	
	if (strcmp(argv[1], "-o") == 0 ) {
		overheadEstimation();
		return 0;
	}
	
	if (strcmp(argv[1], "-r") == 0) {
		mode = 1;
	} else if (strcmp(argv[1], "-w") == 0) {
		mode = 2;
	} else if(strcmp(argv[1], "-rw") == 0) {
		mode = 3;
	} else if(strcmp(argv[1], "-s") == 0) {
		mode = 4;
	} else if(strcmp(argv[1], "-io") == 0) {
		mode = 5;
	} else if(strcmp(argv[1], "-p") == 0) {
		mode = 6;
	}
	helper(atoi(argv[2]), mode);
}
