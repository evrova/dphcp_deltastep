/**
 * @file DeltaStepping.h
 * @brief Main header file for the delta stepping algorithm
 * 
 * @author
 * @date 10/2018
 */

#ifndef DELTA_STEPPING_H
#define DELTA_STEPPING_H

#include <iostream>
#include <stdlib.h>
#include <utility>
#include <vector>
#include <stdint.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <limits>
#include <list>
#include <unordered_map> 

#include "graph.h"

namespace DeltaStepping{
	
	typedef std::vector<Graph::Node> Bucket;
	typedef std::vector<Bucket> BucketSet;
	typedef Graph::Edge Edge;
	typedef Graph::Node Node;
	typedef int Dist;
	
	/**
	 * @brief Runs the deltastepping algorithm
	 * @param graph, the graph to be delta-stepped
	 * @param source Node representing the source
	 * @param delta & delta 
	 * @return sssp result
	 */
	void runDeltaStep(Graph &graph, Node source, const Dist delta, Dist * tent_dists);

	/**
	 * @brief Updates the tentative distance to the node if the one passed as parameter deacreases the distance
	 * @param node
	 * @param new_dist
	 */		
	inline void relax(const Node node, const Dist new_dist, Dist * tent_dists, BucketSet &buckets, const Dist delta, const int numberOfBuckets);

	/**
	 * @brief Finds the smallest index of a non-empty bucket in the "buckets" array
	 */
	inline int find_smallest_nonempty_bucket(const int numberOfBuckets, int lastVisitedBucket, const BucketSet & buckets);

	/**
	 * @brief Computes in which bucket the node falls according to its tentative distance
	 * @param dist
	 * @return int bucket index
	 */
	inline int get_bucket_index(const Dist dist, const Dist delta, const int numberOfBuckets); 

	/**
	 * @brief Prints buckets
	 */
	inline void print_buckets(const BucketSet &buckets);
	
	/**
	 * @brief Prints tentative distances from source node
	 */
	inline void print_dists(const Dist * dists, const int numDists);	
}
#endif
