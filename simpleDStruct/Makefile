CC = /s/gcc-4.8.1/bin/g++
FLAG = -std=c++11

all: list hash arr bench

list: listbench.cpp
	$(CC) $(FLAG) -g -O listbench.cpp -o list -mrtm -lm -lpthread

hash: hashbench.cpp
	$(CC) $(FLAG) -g -O hashbench.cpp -o hash -mrtm -lm -lpthread

arr: arrbench.cpp
	$(CC) $(FLAG) -g -O arrbench.cpp -o arr -mrtm -lm -lpthread

bench: bench.cpp
	$(CC) $(FLAG) -g -O bench.cpp -o bench -mrtm

clean:
	rm bench arr hash list
