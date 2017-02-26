override CFLAGS := -std=gnu11 -march=armv4t -mtune=arm926ej-s -mfloat-abi=soft -Ofast -s -DNDEBUG

default: all

all: ev3-server

clean: clean-ev3-server

.PHONY: ev3-server clean-ev3-server dirtest

ev3-server: main.c
	gcc $(CFLAGS) -o ev3-server main.c

clean-ev3-server:
	rm -f ev3-server

# temporary
dirtest: dirtest.c
	gcc $(CFLAGS) -o dirtest dirtest.c
