#include "DeltaSteppingSeq.h"
#include <iostream>
#include <ostream>
#include <cmath>

using namespace DeltaStepping;

inline int DeltaStepping::get_bucket_index(const Dist dist, const Dist delta, const int numberOfBuckets){
	return ( (uint)floor( (double)dist / (double)delta) ) % numberOfBuckets;
}		
		
inline void DeltaStepping::relax(const Node node, const Dist new_dist, Dist * tent_dists, BucketSet &buckets, const Dist delta, const int numberOfBuckets){		
		
	const Dist curr_dist = tent_dists[node];
	
	//DIFF with prev. version - TODO: change the push back
	if (new_dist < curr_dist){
		const int new_bucket_idx = get_bucket_index(new_dist, delta, numberOfBuckets);
		buckets[new_bucket_idx].push_back(std::move(node)); // move from old into new bucket
		tent_dists[node] = new_dist;
	}
}

inline int DeltaStepping::find_smallest_nonempty_bucket(const int numberOfBuckets, int lastVisitedBucket, const BucketSet & buckets) {
			
	for(int l = 0; l < numberOfBuckets; ++l){
		const int testIndex = (uint)(lastVisitedBucket+l) % (uint)numberOfBuckets;
		if (!(buckets[testIndex].empty())){
			return testIndex;
		}
	}
	return -1;
}

inline void DeltaStepping::print_buckets(const BucketSet &buckets){
	
	std::cout<<"*************BUCKETS************"<<std::endl;	
	for(size_t l = 0; l < buckets.size(); ++l){
		std::cout<<"IN BUCKET "<<l<<": "
		<<buckets[l].size()<<std::endl;
		/*
		Bucket bucket = this->buckets[l];
		if (bucket.empty()){
			std::cout<<"empty"<<std::endl;
		}else{
			std::list<Graph::Node>::iterator bucket_it;
			for(bucket_it = bucket.begin(); bucket_it != bucket.end(); ++bucket_it){
				std::cout<<*bucket_it<<" ";
			}
			std::cout<<std::endl;
		}*/
	}
	std::cout<<"******************************"<<std::endl<<std::endl;
}

inline void DeltaStepping::print_dists(const Dist * dists, const int numDists){
	
	std::cout<<"******TENTATIVE DISTANCES******"<<std::endl;
	int dist;
	for(int i = 0; i < numDists; ++i){
		dist = dists[i];
		if (dist != std::numeric_limits<int>::max()){
				std::cout<<"Node "<<i<<": "<<dist<<std::endl;
		}else{
			std::cout<<"Node "<<i<<": "<<"inf"<<std::endl;
		}
	}
	std::cout<<"******************************"<<std::endl<<std::endl;
}

void init_tent_dists(Dist * tent_dists, int num_nodes){
	for (int i = 0; i < num_nodes; ++i){
		tent_dists[i] = std::numeric_limits<int>::max();
	}
}

void DeltaStepping::runDeltaStep(Graph &graph, DeltaStepping::Node source, Dist delta, DeltaStepping::Dist * tent_dists){
	
	// setup graph
	Edge * _adjacancy 			= graph.getAdjecencyPtr();
	const long num_nodes 		= graph.getNumberOfNodes();
	const long num_arcs  		= graph.getNumberOfArcs();
	Node * numberOfNeigbours 	= graph.getNeighbourPtr();
	Dist maxEdgeWeight 			= graph.getMaxEdgeWeight();
	Dist maxDegree 				= graph.getMaxDegree();
	
	std::cout<<"Max edge weight of the graph: "<<maxEdgeWeight<<std::endl;
	std::cout<<"Max degree of the graph: "<<maxDegree<<std::endl;
	
	// setup delta
	if(delta <= 0){
		delta = maxEdgeWeight/maxDegree;
		std::cout<<"Delta not specified, setting to maxEdgeWeight/maxDegree: "<<delta<<std::endl;
	}

	//setup tentative distances
	init_tent_dists(tent_dists, num_nodes);

	// setup buckets:
	int numberOfBuckets = (maxEdgeWeight/delta) + 2;
	BucketSet buckets(numberOfBuckets);
	Bucket currentBucket; // called removed nodes in the paper

	std::cout << "Total number of buckets: " << numberOfBuckets <<std::endl;

	// setup initial conditions
	relax(source, 0, tent_dists, buckets, delta, numberOfBuckets); // set 0 dist to source
	int smallest_nonempty_bucket = find_smallest_nonempty_bucket(numberOfBuckets, 0, buckets);
	int num_phase = 1;
	
	// begin delta stepping:
	while(smallest_nonempty_bucket != -1){	// while there exists a non-empty bucket
			
			while(!buckets[smallest_nonempty_bucket].empty()){ // while the current bucket is non-empty
				
				//std::cout<<"---------------------------------PHASE "<<num_phase<<"---------------------------------"<<std::endl;
				
				// empty the previous content of currentBucket and replace with 
				currentBucket.clear();
				std::swap(currentBucket, buckets[smallest_nonempty_bucket]);

				// find and relax requests out of current bucket
				for(auto bucket_it = currentBucket.begin(); bucket_it != currentBucket.end(); ++bucket_it){
					
					const Node curr_node = *bucket_it;
					
					for(int i = numberOfNeigbours[curr_node]; i < numberOfNeigbours[curr_node+1]; ++i){
						relax(_adjacancy[i].first, tent_dists[curr_node] + _adjacancy[i].second, tent_dists, buckets, delta, numberOfBuckets);
					}// end of neighboring nodes traversal 
				
				}// end of bucket traversal

				// debugging printouts
				//print_buckets(buckets);
				//print_dists(tent_dists, num_nodes);
				num_phase++;
			}// end inner while loop
			
		smallest_nonempty_bucket = find_smallest_nonempty_bucket(numberOfBuckets, smallest_nonempty_bucket, buckets);
		
	}// end outer while loop
}
