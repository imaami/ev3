default: all

.PHONY: all clean

all:
	+make -C src all

clean:
	+make -C src clean

# temporary
dirtest:
	+make -C src dirtest
