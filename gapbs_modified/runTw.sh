#!/bin/bash

export GRAPH_NAME="twitter"

export OMP_WAIT_POLICY="ACTIVE"
export OMP_DYNAMIC="FALSE"
#export OMP_PROC_BIND="TRUE"
export OMP_SCHEDULE="GUIDED,32"

threads='24 1 2 4 20 6 18 16 10 14 12'

for i in $threads; do
	export OMP_NUM_THREADS=$i
	echo 'Running with' ${OMP_NUM_THREADS} 'threads'
	make bench-run
done
