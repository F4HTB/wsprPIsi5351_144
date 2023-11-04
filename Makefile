LDFLAGS=-lpigpio
CFLAGS=-Wall
CC=gcc
LIBS = -L /usr/local/include -lpigpio -lrt

all: 
	$(CC) $(CFLAGS) wspr.c $(LIBS) -o wspr

clean:
	rm -f wspr
