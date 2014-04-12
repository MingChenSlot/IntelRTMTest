CC = /s/gcc-4.8.1/bin/g++
FLAG = -std=c++11

# OBJ = lrubench.o concurrentLRUCache.o

all: lrubench bench

#lrubench: $(OBJ)
#	$(CC) -g -o lrubench -Werror -pedantic $(OBJ) -lm -lpthread -mrtm
lrubench: lrubench.cpp
	$(CC) $(FLAG) -g -o lrubench -O lrubench.cpp -lm -lpthread -mrtm

bench: bench.cpp
	$(CC) $(FLAG) -g -O bench.cpp -o bench -mrtm

#lrubench.o: lrubench.cpp
#	$(CC) $(FLAG) -g -c lrubench.cpp -o arr -mrtm -lm -lpthread

#concurrentLRUCache.o: concurrentLRUCache.cpp
#	$(CC) $(FLAG) -g -c concurrentLRUCache.cpp -o arr -mrtm -lm -lpthread

clean:
	rm lrubench bench 
