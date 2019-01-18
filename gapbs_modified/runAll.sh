#!/bin/bash

threads='1 2 4 8'

for i in $threads; do
	export OMP_NUM_THREADS=$i
	echo 'Running with' ${OMP_NUM_THREADS} 'threads'
	make bench-run
done
