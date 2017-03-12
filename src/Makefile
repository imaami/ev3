override CFLAGS := -std=gnu11 -march=armv4t -mtune=arm926ej-s -mfloat-abi=soft -Ofast -s -DNDEBUG

override EV3_SERVER_OBJ := \
	main.o \
	ev3_server.o \
	ev3_io.o \
	ev3_driver_color.o \
	ev3_driver_ir.o \
	ev3_driver_l_motor.o \
	ev3_driver_m_motor.o \
	ev3_driver_touch.o

default: all

all: ev3-server

clean: clean-ev3-server

.PHONY: ev3-server clean-ev3-server

%.o: %.c
	gcc -c $(CFLAGS) $< -o $@

ev3-server: $(EV3_SERVER_OBJ)
	gcc $(CFLAGS) -o ev3-server $(EV3_SERVER_OBJ)

clean-ev3-server:
	rm -f ev3-server *.o
