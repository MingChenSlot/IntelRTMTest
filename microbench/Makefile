CC = /s/gcc-4.8.1/bin/g++

all: multi bench

bench: bench.cpp
	$(CC) -g -O bench.cpp -o bench -mrtm

multi: multi_t_bench.cpp
	$(CC) -g -O multi_t_bench.cpp -o multi -mrtm -lm -lpthread

clean:
	rm bench multi
