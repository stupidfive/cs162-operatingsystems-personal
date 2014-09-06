default: main wc map

main: main.c
	gcc -g main.c -o main

wc: wc.c
	gcc -g wc.c -o wc

map: map.c
	gc -g map.c -o map

check: check_wc check_main

check_wc:
	./wc | grep 1

check_main:
	./main | grep 1

clean:
	rm wc main map
