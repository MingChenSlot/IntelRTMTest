/**
* file:concurrentLRUCache.h
* @author: Ming Chen 9068207811 <mchen67@wisc.edu>
* @version 1.0
* @created time: Thu 10 Apr 2014 05:24:48 PM CDT
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

#ifndef CONCURRENTLRUCACHE_H
#define CONCURRENTLRUCACHE_H


#include <list>
#include <unordered_map>
#include "synclib.h"
using namespace std;

template<class K, class T>
class ConcurrentLRUCache {
private:
	unordered_map< K, typename list<T>::iterator > _mapping;
	list<T> lru;
	unsigned int cache_size;
	Lock l;

public:
	ConcurrentLRUCache (unsigned int size = 10) {
		cache_size = size;	
	}

	~ConcurrentLRUCache () {
		_mapping.clear();
		lru.clear();
	}
	
	void put (K key, T data) {
		if (_mapping.count(key)) {
			lru.erase(_mapping[key]);
		} else if (lru.size() == cache_size) {
			// for complete implementation, should write data into 
			// lower level storage, here we just delete the data
			lru.pop_back();
		}
		// else is that cache is not fulll yet
		lru.push_back(data);
		_mapping[key] = (--lru.end());
	}

	T get (K key) {
		T value;
		if (_mapping.count(key)) {
			value = *(_mapping[key]);
			/*
			cout << "key " << key << endl;
			cout << "value " << value << endl;
			cout << "begin lru: " << *(lru.begin()) << endl;
			cout << "end lru: " << *(lru.rbegin()) << endl;
			*/
			// update LRU
			lru.erase(_mapping[key]);
			lru.push_back(value);
			_mapping[key] = (--lru.end());
		} 
		// should go to lower level storage to get corresponding
		// data
		return value;	
	}

	void lockPut (K key, T data) {
		l.acquire();
		put (key, data);
		l.release();
	}

	T lockGet (K key) {
		l.acquire();
		get (key);
		l.release();
	}

};
	
#endif
