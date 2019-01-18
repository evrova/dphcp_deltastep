#include "DeltaStepping.h"
#include <iostream>
#include <ostream>
#include <cmath>
#include <algorithm>
#include <random>

#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_num_threads() 1
#define omp_get_thread_num() 0
#endif

using namespace DeltaStepping;

inline int 
DeltaStepping::get_bucket_index(const Dist & dist, const Dist & delta, const int & numberOfBuckets){
	return (dist/delta) % numberOfBuckets;
}		

inline void 
DeltaStepping::relax(const Node & node, const Dist & new_dist, Dist * &tent_dists, BucketSet& buckets, const Dist & delta, const int & numberOfBuckets){		
		
	const Dist curr_dist = tent_dists[node];
	if (new_dist < curr_dist){
		const int new_bucket_idx = get_bucket_index(new_dist,delta,numberOfBuckets);
		buckets[new_bucket_idx].push_back(node); // insert into new bucket
		tent_dists[node] = new_dist;
	}
}

/*
inline int 
DeltaStepping::find_smallest_nonempty_bucket(const int & numberOfBuckets, int & lastVisitedBucket, const BucketSet & buckets) {
			
	for(int l = 0; l < numberOfBuckets; ++l){
		const int testIndex = (uint)(lastVisitedBucket+l) % (uint)numberOfBuckets;
		if (!(buckets[testIndex].empty())){
			lastVisitedBucket = testIndex;
			return testIndex;
		}
	}
	return -1;
}
*/

// must be called within omp parallel
inline int 
DeltaStepping::find_smallest_nonempty_bucket(const int & numberOfBuckets, const BucketSet & buckets, const int & lastVisitedBucket, int & reductionVariable) {

	// todo: improve currently lets all threads run untill they have found the smallest entry
	int smallestBucket = numberOfBuckets+1;

	for(int l = 0; l < numberOfBuckets; ++l){
		const int testIndex = (lastVisitedBucket+l) % numberOfBuckets;
		if (!(buckets[testIndex].empty())){
			smallestBucket = l;
			break;
		}
	}

	#pragma omp single
	reductionVariable = numberOfBuckets+1;

	//#pragma omp flush(reductionVariable)
	//{
	//	if (reductionVariable > smallestBucket)
		#pragma omp critical
		{
			if (reductionVariable > smallestBucket)
				reductionVariable = smallestBucket;
		}
	//}
	    
	#pragma omp barrier
	smallestBucket = reductionVariable;

	if (smallestBucket==numberOfBuckets+1)
		smallestBucket = -1;
	else
		smallestBucket = (lastVisitedBucket+smallestBucket) % numberOfBuckets;

	return smallestBucket;
}

inline void 
DeltaStepping::print_buckets(const BucketSet &buckets){
	std::cout<<"*************BUCKETS************"<<std::endl;	
	for(size_t l = 0; l < buckets.size(); ++l){
		std::cout<<"IN BUCKET "<<l<<": ";
		//<<buckets[l].size()<<std::endl;
		
		Bucket bucket = buckets[l];
		if (bucket.empty()){
			std::cout<<"empty"<<std::endl;
		}else{
			for(auto bucket_it = bucket.begin(); bucket_it != bucket.end(); ++bucket_it){
				std::cout<<*bucket_it<<" ";
			}
			std::cout<<std::endl;
		}
	}
	std::cout<<"******************************"<<std::endl<<std::endl;
}

inline void 
DeltaStepping::print_dists(const Dist * dists, const int &numDists){
	
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

Dist *
DeltaStepping::runDeltaStep(const Graph &graph, DeltaStepping::Node source, const Dist & delta){
	
	// setup graph:
	const long num_nodes 		= graph.getNumberOfNodes();
	//const long num_arcs  		= graph.getNumberOfArcs();
	Edge * _adjacancy 			= graph.getAdjecencyPtr();
	NodeOffset * numberOfNeigbours = graph.getNeighbourPtr();
	Dist maxEdgeWeight 			= graph.getMaxEdgeWeight();
	const Dist thisDelta		= delta;

	// setup tenative distances:
	Dist * tent_dists = new Dist[num_nodes];
	#pragma omp parallel for schedule(static)
	for (int i = 0; i < num_nodes; ++i){
		tent_dists[i] = std::numeric_limits<int>::max();
	}

	bool * visitedNodesFlags = new bool[num_nodes];
	#pragma omp parallel for schedule(static)
	for (int i = 0; i < num_nodes; ++i){
		visitedNodesFlags[i] = false;
	}

	int numThreads;
	#pragma omp parallel
	#pragma omp single
	numThreads = omp_get_num_threads();
	//std::cout << "Number of threads: " << numThreads << std::endl;

	// setup ownership array
	int * owners = new int[num_nodes];
	#pragma omp parallel
	{
		//~ std::random_device rd;     // only used once to initialise (seed) engine 
		//~ std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case) 
		//~ std::uniform_int_distribution<int> uni(0,numThreads-1); // guaranteed unbiased 
		
		#pragma omp for schedule(static)
		for (int i = 0; i < num_nodes; ++i){
			
			owners[i] = i % numThreads;
			//~ owners[i] = uni(rng); //todo: set random proc
		}
	}

	const int rqStride = 64;
	RequestQueue ** sharedRequestQueue = new RequestQueue * [(numThreads-1)*rqStride+1];
	#pragma omp parallel for schedule(static)
	for (int i=0; i<numThreads; ++i)
		sharedRequestQueue[i*rqStride] = new RequestQueue  [(numThreads-1)*rqStride+1];


	// setup buckets:
	const int numberOfBuckets = (maxEdgeWeight/delta) + 2;
	int sharedReductionVariable;
	//Bucket currentBucket; // called removed nodes in the paper

	//std::cout << "number of buckets: " << numberOfBuckets <<std::endl;

	bool currentBucketIsEmpty = false;

	#pragma omp parallel firstprivate(numberOfBuckets, _adjacancy, numberOfNeigbours, tent_dists, thisDelta, owners, rqStride)
	{
		const int ti = omp_get_thread_num();
		const int localNumThreads = numThreads;

		BucketSet buckets(numberOfBuckets);
		Bucket visitedNodes;

		RequestQueue ** requestQueue = new RequestQueue * [localNumThreads];
		//copying from strided requests to local requestQueue
		for (int i=0; i<localNumThreads; ++i){
			requestQueue[i] = sharedRequestQueue[i*rqStride];
		}

		for (int i=0; i<localNumThreads; ++i){
			requestQueue[ti][i*rqStride].reserve(1024);
		}

		// setup initial conditions
		if (owners[source] == ti){
			relax(source,0,tent_dists,buckets,thisDelta,numberOfBuckets); // set 0 dist to source
		}

		int smallest_nonempty_bucket =
		find_smallest_nonempty_bucket(numberOfBuckets,buckets,0,sharedReductionVariable);

		int num_phase = 1;
		
		// begin delta stepping:
		while(smallest_nonempty_bucket != -1){	// while there exists a non-empty bucket
				while(!currentBucketIsEmpty){ // while the current bucket is non-empty
					//#pragma omp single
					//std::cout<<"---------------------------------PHASE "<<num_phase<<"---------------------------------"<<std::endl;
					// empty the previous content of currentBucket and replace with 

					// find requests out of own currrOfBuckets+1ent bucket
					const uint currentBucketSize = buckets[smallest_nonempty_bucket].size();
					for(uint i = 0; i < currentBucketSize; ++i){

						const Node curr_node = buckets[smallest_nonempty_bucket][i];

						if (i != currentBucketSize-1)
						{
							const Node nn = buckets[smallest_nonempty_bucket][i+1];
							__builtin_prefetch (&tent_dists[nn], 0, 2);
							__builtin_prefetch (&numberOfNeigbours[nn], 0, 2);
						}
						
						const Graph::Weight curr_dist = tent_dists[curr_node];

						const NodeOffset startAdj = numberOfNeigbours[curr_node];
						const NodeOffset endAdj   = numberOfNeigbours[curr_node+1];

						for(NodeOffset j = startAdj; j < endAdj; ++j){
							const Node requestNode = _adjacancy[j].first;
							const Graph::Weight requestWeight = _adjacancy[j].second;

							if (j < endAdj-2)
							{
								__builtin_prefetch (&owners[_adjacancy[j+2].first], 0, 1);
							}

							if (requestWeight < delta)
							{
								const int requestOwner = owners[requestNode];
								requestQueue[ti][ requestOwner*rqStride ].emplace_back(requestNode, curr_dist+requestWeight);
							}
							
						}// end of neighboring nodes traversal

						if (!visitedNodesFlags[curr_node])
						{
							visitedNodesFlags[curr_node] = true;
							visitedNodes.push_back(curr_node);
						}

					}// end of bucket traversal
					buckets[smallest_nonempty_bucket].clear();

					#pragma omp barrier
					
					// relax requests
					for (int i=0; i<localNumThreads; ++i){
						const auto endIt = requestQueue[i][ti*rqStride].end();
						for(auto it = requestQueue[i][ti*rqStride].begin(); it != endIt; ++it){
							relax(it->first, it->second, tent_dists, buckets, thisDelta, numberOfBuckets);
						}
						requestQueue[i][ti*rqStride].clear();
					}

					#pragma omp single
					currentBucketIsEmpty = true;

					#pragma omp critical
					currentBucketIsEmpty = currentBucketIsEmpty && buckets[smallest_nonempty_bucket].empty();

					num_phase++;					
					#pragma omp barrier
				}// end inner while loop

			// process heavy edges
			const uint visitedNodesSize = visitedNodes.size();
			for(uint i = 0; i < visitedNodesSize; ++i){
				const Node curr_node = visitedNodes[i];

				if (i != visitedNodesSize-1)
				{
					const Node nn = visitedNodes[i+1];
					__builtin_prefetch (&tent_dists[nn], 0, 2);
					__builtin_prefetch (&numberOfNeigbours[nn], 0, 2);
				}
				
				const Graph::Weight curr_dist = tent_dists[curr_node];
				const NodeOffset startAdj = numberOfNeigbours[curr_node];
				const NodeOffset endAdj   = numberOfNeigbours[curr_node+1];

				for(NodeOffset j = startAdj; j < endAdj; ++j){
					const Node requestNode = _adjacancy[j].first;
					const Graph::Weight requestWeight = _adjacancy[j].second;

					if (j < endAdj-2)
					{
						__builtin_prefetch (&owners[_adjacancy[j+2].first], 0, 1);
					}

					if (requestWeight >= delta)
					{
						const int requestOwner = owners[requestNode];
						requestQueue[ti][ requestOwner*rqStride ].emplace_back(requestNode, curr_dist+requestWeight);
					}	
				}// end of neighboring nodes traversal
			}// end of bucket traversal
			buckets[smallest_nonempty_bucket].clear();

			#pragma omp barrier
			
			// relax requests
			for (int i=0; i<localNumThreads; ++i){
				const auto endIt = requestQueue[i][ti*rqStride].end();
				for(auto it = requestQueue[i][ti*rqStride].begin(); it != endIt; ++it){
					relax(it->first, it->second, tent_dists, buckets, thisDelta, numberOfBuckets);
				}
				requestQueue[i][ti*rqStride].clear();
			}

			visitedNodes.clear();
			

			smallest_nonempty_bucket =
			find_smallest_nonempty_bucket(numberOfBuckets,buckets,smallest_nonempty_bucket,sharedReductionVariable);

			#pragma omp single
			currentBucketIsEmpty = false;
		}// end outer while loop

		delete[] requestQueue;
	}

	delete[] owners;

	/*
	std::cout << "Request queue capacities: " << std::endl;
	for (int i=0; i<numThreads; ++i)
	{
		for (int j=0; j<numThreads; ++j)
			std::cout << sharedRequestQueue[i*rqStride][j*rqStride].capacity() << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
	*/
	
	
	for (int i=0; i<numThreads; ++i)
		delete[] sharedRequestQueue[i*rqStride];
	delete[] sharedRequestQueue;

	return tent_dists;
}
