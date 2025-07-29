
CC = gcc
AR = ar 
CFLAGS = -Wall -fPIC

all: static shared 

static: libstrutils.a main_static

shared: libstrutils.so main_shared

libstrutils.a: bstrutils.o 
	$(AR) rcs libstrutils.a bstrutils.o

libstrutils.so: bstrutils.o
	$(CC) -shared -o libstrutils.so bstrutils.o

bstrutils.o: bstrutils.c strutils.h
	$(CC) $(CFLAGS) -c bstrutils.c  

main_static: main.c strutils.h 
	$(CC) main.c -L. -lstrutils -o main_static 

main_shared: main.c strutils.h
	$(CC) main.c -L. -lstrutils -o main_shared

clean:
	rm -f *.o *.a *.so main_static main_shared

