override CFLAGS := -std=gnu11 -march=armv4t -mtune=arm926ej-s -mfloat-abi=soft -Ofast -s -DNDEBUG

default: all

all: ev3-server

clean: clean-ev3-server

.PHONY: ev3-server clean-ev3-server

ev3_server.o: ev3_server.c
	gcc $(CFLAGS) -c -o ev3_server.o ev3_server.c

ev3_io.o: ev3_io.c
	gcc $(CFLAGS) -c -o ev3_io.o ev3_io.c

main.o: main.c
	gcc $(CFLAGS) -c -o main.o main.c

ev3-server: main.o ev3_server.o ev3_io.o
	gcc $(CFLAGS) -o ev3-server main.o ev3_server.o ev3_io.o

clean-ev3-server:
	rm -f ev3-server *.o
