# Delta-Stepping Algorithm - Solving SSSP on Shared Memory

## General Information
This repository contains the code for the project of Design of Parallel and High Performance Computing course at ETHZ, Fall 2018. We have used the [GAP benchmark suite](https://github.com/sbeamer/gapbs) and modified their framework in order to easily benchmark against their implementation and reproduce the results.

We have used predetermined values by GAP for value of delta for the different graphs.

## Reproducing results

To run the benchmarks:

- compile the code with `make`
- compile the code in the `gapbs_modified` directory with `make`
- run the `runRo.sh`, `runKr`, `runUr` or `runTw` scripts for the desired graph in the `gapbs_modified` directory or setup the `benh.mk` file in `gapbs_modified/benchmark` directory and run the command `make bench-run`.

To run the shared bucket version, switch to the sharedBucketsVersion branch.

The project group consists of: Erik Träff, Clara Brimnes Gardner, Andrey Oblupin and Gabriela Evrova. 
