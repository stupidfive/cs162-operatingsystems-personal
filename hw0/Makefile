default: main wc map

main: main.c
	gcc -g main.c -o main

wc: wc.c
	gcc -g wc.c -o wc

map: map.c
	gcc -g map.c -o map

check: check_wc check_main

check_wc:
	./check_wc.sh

check_main:
	if ./main | grep stack | grep "$(ulimit -s)"; then echo "stack correct"; else echo "stack incorrect"; fi
	if ./main | grep process | grep "$(ulimit -u)"; then echo "processes correct"; else echo "processes incorrect"; fi
	if ./main | grep descriptors | grep "$(ulimit -n)"; then echo "file descriptors correct"; else echo "file descriptors incorrect"; fi

clean:
	rm wc main map
