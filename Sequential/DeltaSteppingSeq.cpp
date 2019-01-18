#include "DeltaSteppingSeq.h"
#include <iostream>
#include <ostream>
#include <cmath>

// constructor
DeltaStepping::DeltaStepping(Graph &graph, int delta){

	//added
	this->num_nodes = graph.getNumberOfNodes();
	this->num_arcs  = graph.getNumberOfArcs();
	
	this->DELTA = delta;

	this->_adjacancy = graph.getAdjecencyPtr();
	this->numberOfNeigbours = graph.getNeighbourPtr();

	this->maxEdgeWeight = graph.getMaxEdgeWeight();
	this->numberOfBuckets = (uint)ceil( (double)maxEdgeWeight/(double)delta ) + 1;

	std::cout <<"number of buckets: " << numberOfBuckets <<std::endl;
	
	this->tent_dists = std::vector<Dist>(num_nodes);
	this->init_tent_dists();
	
	// init buckets
	this->buckets = std::vector<Bucket>(this->numberOfBuckets);

	this->nodeHasBeenDeleted = new bool[this->num_nodes];
	for (int i=0; i<this->num_nodes; ++i)
		this->nodeHasBeenDeleted[i] = false;
}

DeltaStepping::~DeltaStepping(){
	delete[] this->nodeHasBeenDeleted;
}

// initialization of tentative distances
void DeltaStepping::init_tent_dists(){

	 size_t size = tent_dists.size();
	 
	 for (size_t i = 0; i < size; ++i){
		tent_dists[i] = std::numeric_limits<int>::max();
	}
}

int DeltaStepping::get_bucket_index(const int & dist) const{
	return ( (uint)floor( (double)dist / this->DELTA) ) % (this->numberOfBuckets);
}		
		
// update tentative distances
void DeltaStepping::relax(const Node & node, const Dist & new_dist){		
		
	const Dist curr_dist = this->tent_dists[node];
	if (new_dist < curr_dist){

		const int new_bucket_idx = get_bucket_index(new_dist);
		
		// if the distance is inf, the node is not in a bucket
		if(curr_dist != std::numeric_limits<int>::max())
		{
			const int curr_bucket_idx = get_bucket_index(curr_dist);
			if (curr_bucket_idx != new_bucket_idx)
			{
				
				this->buckets[curr_bucket_idx].remove(node); // remove node from old bucket	
			}			
		}

		this->buckets[new_bucket_idx].push_back(std::move(node)); // insert into new bucket
		this->tent_dists[node] = new_dist;
	}
}
	
// relax Requests
void DeltaStepping::relax_requests(std::vector<Request> &requests){
	
	for(auto it = requests.begin(); it != requests.end(); ++it){
		
		// a Request is a tuple of (node, new_dist)
		this->relax(it->first, it->second);
	}
}

//~ Request makeRequest(){
	//~ //todo
//~ }
	
// generating requests for the nodes we are interested in
// write the requests to the list "requests" passed as parameter
void DeltaStepping::find_requests(std::vector<Request> &requests, Bucket &bucket, const EdgeType & edge_type){

	//TODO: in the future we should use separate graphs for light and heavy edges, perhaps
	//TODO: split into two functions, as edgetype is always hardcoded

	// check if bucket is empty
	if (!bucket.empty()){
		std::list<Node>::iterator bucket_it;

		switch(edge_type){
			case LIGHT:
				for(bucket_it = bucket.begin(); bucket_it != bucket.end(); ++bucket_it){
					const Node curr_node = *bucket_it;
					
					// 1. traverse the neighbors of current node
					// 2. find the edges of interest (light/heavy)
					// 3. generate request
					for(int i = numberOfNeigbours[curr_node]; i < numberOfNeigbours[curr_node+1]; ++i){
						if (_adjacancy[i].second <= this->DELTA){ // if node is light
							requests.emplace_back(_adjacancy[i].first, 
							this->tent_dists[curr_node] + _adjacancy[i].second);
						}
					}// end of neighboring nodes traversal
				}// end of bucket traversal
			break;

			case HEAVY:
				for(bucket_it = bucket.begin(); bucket_it != bucket.end(); ++bucket_it){
					const Node curr_node = *bucket_it;
					
					// 1. traverse the neighbors of current node
					// 2. find the edges of interest (light/heavy)
					// 3. generate request
					for(int i = numberOfNeigbours[curr_node]; i < numberOfNeigbours[curr_node+1]; ++i){
						if (_adjacancy[i].second > this->DELTA){ // if node is heavy
							requests.emplace_back(_adjacancy[i].first, 
							this->tent_dists[curr_node] + _adjacancy[i].second);
						}
					}// end of neighboring nodes traversal
				}// end of bucket traversal
			break;
		} // end swich heavy / light
	} // end if bucket non-empty
}//end of function

int DeltaStepping::find_smallest_nonempty_bucket() {
		
	const size_t len = this->numberOfBuckets;
	
	for(size_t l = 0; l < len; ++l){
		const int testIndex = (uint)(this->lastVisitedBucket+l) % (uint)this->numberOfBuckets;
		if (!(this->buckets[testIndex].empty())){
			this->lastVisitedBucket = testIndex;
			return testIndex;
		}
	}
	return -1;
}

void 
DeltaStepping::emptyBucket(Bucket & bucket, Bucket & visitedNodes)
{
	for (auto it=bucket.begin(); it != bucket.end(); ++it)
	{
		if (!this->nodeHasBeenDeleted[*it])
		{
			visitedNodes.push_back(std::move(*it));
			this->nodeHasBeenDeleted[*it] = true;
		}
	}
	bucket.clear();
}

int DeltaStepping::check_input(const Graph::Node & source) const{
	
	//TODO error messages
	if(&(this->_adjacancy) == nullptr){
		return -1;
	}
	
	if(source < 0){
		return -2;
	}
	
	if(this->DELTA < 0){
		return -3;
	}
	
	return 0;
}

void DeltaStepping::print_buckets() const{
	
	std::cout<<"*************BUCKETS************"<<std::endl;
	
	size_t len = this->buckets.size();
	
	
	for(size_t l = 0; l < len; ++l){
		
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
		}
		*/
		
	}
	std::cout<<"******************************"<<std::endl<<std::endl;
	
}

void DeltaStepping::print_dists() const{
	
	std::cout<<"******TENTATIVE DISTANCES******"<<std::endl;
	
	size_t len = this->tent_dists.size();
	int dist;
	
	for(size_t i = 0; i < len; ++i){
		
		dist = this->tent_dists[i];
		if (dist != std::numeric_limits<int>::max()){
				std::cout<<"Node "<<i<<": "<<dist<<std::endl;
		}else{
			std::cout<<"Node "<<i<<": "<<"inf"<<std::endl;
		}
	}
	std::cout<<"******************************"<<std::endl<<std::endl;
}

std::vector<DeltaStepping::Dist> 
DeltaStepping::run(Node source){
	
	int err = check_input(source);
	
	//std::cout<<err<<std::endl;
	
	if(err != 0){
		std::perror("ERROR in the input parameters!\n");
		std::cout<<std::flush;
	}
	
	// set 0 dist to source
	relax(source,0);
	
	int smallest_nonempty_bucket = find_smallest_nonempty_bucket();
	std::vector<Request> requests;
	
	Bucket visited_nodes; // called removed nodes in the paper
	
	int num_phase = 1;
	
	while(smallest_nonempty_bucket != -1){	
		
			// redo
			while(!this->buckets[smallest_nonempty_bucket].empty()){
				
				//std::cout<<"---------------------------------PHASE "<<num_phase<<"---------------------------------"<<std::endl;
				
				// redo
				this->find_requests(requests, this->buckets[smallest_nonempty_bucket], EdgeType::LIGHT);
				
				// Move all elements from bucket to visited_nodes
				emptyBucket(this->buckets[smallest_nonempty_bucket], visited_nodes);
				//visited_nodes.splice(std::end(visited_nodes),this->buckets[smallest_nonempty_bucket]);
				//std::cout << "empty bucket size: " << buckets[smallest_nonempty_bucket].empty() << std::endl;
				
				this->relax_requests(requests);
				requests.clear();
				
				//print_buckets();
				//print_dists();
			
				//std::cout<<"-------------------------------------------------------------------------"<<std::endl;
				num_phase++;
				
			}// end inner while loop			
			
		this->find_requests(requests, visited_nodes, EdgeType::HEAVY);
		this->relax_requests(requests);
		requests.clear();
		visited_nodes.clear();

		// find smallest non-empty bucket for next iteration
		smallest_nonempty_bucket = find_smallest_nonempty_bucket();
		
	}// end outer while loop

	return tent_dists;
}
