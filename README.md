# Delta-Stepping Algorithm - Solving SSSP on Shared Memory

## General Information
This repository contains the code for the project of Design of Parallel and High Performance Computing course at ETHZ, Fall 2018. We have used the [GAP benchmark suite](https://github.com/sbeamer/gapbs) and modified their framework in order to easily benchmark against their implementation and reproduce the results.


To rerun the benchmarks:

- compile the code with `make`
- compile the code in the `gabbs_modified` directory with `make`
- run the `make-bench` command in the `gapbs_modified` directory

To run the shared bucket version, switch to the sharedBucketsVersion branch.
