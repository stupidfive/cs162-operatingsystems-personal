#!/bin/bash

# pte test
if [ -n "$(./pagetable | grep "PTE size: 4 bytes")" ]; then echo "Correct PTE size of 4 bytes"; else echo "Incorrect PTE size"; fi

# correct evictions without clock algorithm
./pagetable | grep evictions > mysol
./pt-soln | grep evictions > sol
if [ -z $(diff sol mysol) ]; then echo "No input passes"; else echo "No input does not pass"; fi

./pagetable 1 | grep evictions > mysol
./pt-soln 1 | grep evictions > sol
if [ -z $(diff sol mysol) ]; then echo "1 input passes"; else echo "1 input does not pass"; fi

./pagetable 2 | grep evictions > mysol
./pt-soln 2 | grep evictions > sol
if [ -z $(diff sol mysol) ]; then echo "2 input passes"; else echo "2 input does not pass"; fi

./pagetable 3 | grep evictions > mysol
./pt-soln 3 | grep evictions > sol
if [ -z $(diff sol mysol) ]; then echo "3 input passes"; else echo "3 input does not pass"; fi

# correct evictions with clock algorithm
./pagetable 0 1 | grep evictions > mysol
./pt-soln 0 1 | grep evictions > sol
if [ -z $(diff sol mysol) ]; then echo "0 1 input passes"; else echo "0 1 input does not pass"; fi

./pagetable 1 1 | grep evictions > mysol
./pt-soln 1 1 | grep evictions > sol
if [ -z $(diff sol mysol) ]; then echo "1 1 input passes"; else echo "1 1 input does not pass"; fi

./pagetable 2 1 | grep evictions > mysol
./pt-soln 2 1 | grep evictions > sol
if [ -z $(diff sol mysol) ]; then echo "2 1 input passes"; else echo "2 1 input does not pass"; fi

./pagetable 3 1 | grep evictions > mysol
./pt-soln 3 1 | grep evictions > sol
if [ -z $(diff sol mysol) ]; then echo "3 1 input passes"; else echo "3 1 input does not pass"; fi

rm mysol sol
