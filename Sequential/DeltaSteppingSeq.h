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

class DeltaStepping{
	
	//todo consts
	//TODO review all the parameters and pointers
	
	public:
	
		long num_nodes;
		long num_arcs;

		// make life easier
		typedef Graph::Edge Edge;
		typedef Graph::Node Node;
		typedef int Dist;
		
		/**
		 * @brief Constructor of the DeltaStepping class
		 * @param graph A pointer to the graph
		 * @param source Source node ID
		 * @param delta
		 */
		
		DeltaStepping(Graph &graph, int delta);
		
		/**
		* @brief Destructor of the DeltaStepping class
		*/
		~DeltaStepping();
		
		/**
		 * @brief Runs the delta stepping algorithm
		 * @param source Node representing the source
		 */
		std::vector<Dist> 
		run(Graph::Node source);
		
	private:	
	
		Edge * _adjacancy;
		int  * numberOfNeigbours;

		Node source;
		double DELTA; //TODO: put default value max_edge_weight / max_degree;
		
		typedef std::list<Graph::Node> Bucket;
		typedef std::pair<Graph::Node, Dist> Request;
		
		enum EdgeType {LIGHT, HEAVY};
	
		std::vector<Dist> tent_dists; // TODO ASK
		std::vector<Bucket> buckets; // TODO ASK for push back copying
		
		int maxEdgeWeight=0;
		uint numberOfBuckets = 0;
		uint lastVisitedBucket = 0;

		bool * nodeHasBeenDeleted = nullptr;
		
		// how many elements should the vector have? read about recopying
		
		/**
		 * @brief Updates the tentative distance to the node if the one passed as parameter deacreases the distance
		 * @param node
		 * @param new_dist
		 */		
		void relax(const Graph::Node & node, const Dist & new_dist);
		
		
		/**
		 * @brief Tries to relax the requests
		 * @param requests A list of requests
		 */	
		void relax_requests(std::vector<Request> &requests);
		
		
		/**
		 * @brief Generates requests
		 * @param requests A list where the requests will be inserted
		 * @param bucket
		 * @param edge_type Denotes the tye of the edges to be relaxed: Light or Heavy
		 */	
		void find_requests(std::vector<Request> &requests, Bucket &bucket, const EdgeType & edge_type);
		
		
		/**
		 * @brief Finds the smallest index of a non-empty bucket in the "buckets" array
		 */
		int find_smallest_nonempty_bucket();
		
		
		/**
		 * @brief Initializes all distances to inf
		 */
		void init_tent_dists();
		
		/**
		 * @brief Initializes empty buckets
		 */
		void init_buckets();

		void 
		emptyBucket(Bucket & bucket, Bucket & visitedNodes);
		
		/**
		 * @brief Checks if the graph, source node and delta have valid values
		 * @param source Source node
		 */
		int check_input(const Graph::Node & source) const;
		
		/**
		 * @brief Computes in which bucket the node falls according to its tentative distance
		 * @param dist
		 * @return int bucket index
		 */
		int get_bucket_index(const int & dist) const;
		
		/**
		 * @brief Prints buckets
		 */
		void print_buckets() const;
		
		/**
		 * @brief Prints tentative distances from source node
		 */
		void print_dists() const;

};
#endif
