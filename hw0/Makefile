C=gcc
CFLAGS=-I.
EXECUTABLE=main wc map
all: $(EXECUTABLE)

main: main.c
wc: wc.c
map: map.c
check: all
	./check.sh
clean:
	rm -f $(EXECUTABLE)
