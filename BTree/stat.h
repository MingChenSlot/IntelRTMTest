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

#include <vector>
#define NUM_ABORTS 8

using namespace std;

class Statistic {
	vector<int> counter;
	void init() {
		counter.clear();
		counter.resize(NUM_ABORTS);
	}

public:
	Statistic() {
		init();
	}

	~Statistic() {
		clear();
	}

	void add_data(int status) {
		if (status == _XBEGIN_STARTED) {
			counter[0]++;
		} else if (status == _XABORT_EXPLICIT) {
			counter[1]++;
		} else if (status == _XABORT_RETRY) {
			counter[2]++;
		} else if (status == _XABORT_CONFLICT) {
			counter[3]++;
		} else if (status == _XABORT_CAPACITY) {
			counter[4]++;
		} else if (status == _XABORT_DEBUG) {
			counter[5]++;
		} else if (status == _XABORT_NESTED) {
			counter[6]++;
		} else {
			counter[7]++;
		}
	}
	
	void print_stat(int arr_size) {
		if (arr_size < 1024) 
			cout << arr_size << " Bytes" << endl;
		else {
			cout << arr_size / 1024 << " KB" << endl;
		}

		cout << "\tSucc" << "\t";
		cout << "Exp"  << "\t";
		cout << "Retry" << "\t";
		cout << "Confl" << "\t";
		cout << "Cap" 	<< "\t";
		cout << "Debug" << "\t";
		cout << "Nest"  << "\t";
		cout << "Other" << "\n";
		for (int i = 0; i < NUM_ABORTS; ++i) {
			std::cout << "\t" << counter[i];
		}
		cout << "\n";
		init();
	}

	void clear() {
		counter.clear();
	}
};
	
