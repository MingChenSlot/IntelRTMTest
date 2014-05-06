CC = /s/gcc-4.8.1/bin/gcc
FLAG = -D STANDALONE  # -std=c99 # -std=c++11

all: lbt tsxbt nocbt

lbt: threads2j.c
	$(CC) $(FLAG) -g -O threads2j.c -o lbt -mrtm -lm -lpthread

tsxbt: tsx2j.c
	$(CC) $(FLAG) -g -O tsx2j.c -o tsxbt -mrtm -lm -lpthread

nocbt: noc2j.c
	$(CC) $(FLAG) -g -O noc2j.c -o nocbt -mrtm -lm -lpthread

clean:
	rm lbt tsxbt nocbt *.db

