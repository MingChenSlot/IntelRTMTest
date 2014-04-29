CC = /s/gcc-4.8.1/bin/gcc 

FLAG = -D STANDALONE

all: lbt # tsxbt 

lbt: threads2j.c
	$(CC) $(FLAG) -g -O threads2j.c -o lbt -mrtm -lm -lpthread

clean:
	rm lbt 
