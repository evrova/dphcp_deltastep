#include "graph_boost.hpp"
# include <chrono>

void print_distances(std::vector<int> &distmap){
	
	std::vector<int>::iterator it;
	
	for(it = distmap.begin(); it != distmap.end(); ++it){
		std::cout<<*it<<std::endl;
	}
}


int main(int argc, char** argv)
{
	std::cout << "HI!"<< std::endl;
    //~ std::string fileName("../graphs/ny.wsg");
    std::string fileName("../graphs/USA-road-d.NY.gr");
    int delta = 1;
    int numRuns = 1;

    for (int i = 1; i < argc; i++) {
        std::string option(argv[i]);
        if (option==std::string("-f")) {
            fileName.clear();
            fileName = argv[i+1];
        } else if (option==std::string("-d")) {
            delta = atoi(argv[i + 1]);
        } else if (option==std::string("-n")) {
            numRuns = atoi(argv[i + 1]);
        }
    }
	

    printf("\nfileName:\t %s",fileName.c_str());
    printf("\nDelta: \t\t %d\n",delta);
    
    boost_lib::Graph G; 
    boost_lib::WeightMap weight_map = boost::get(boost::edge_weight, G);
    
    long num_nodes = boost_lib::readFromGR(fileName, G, weight_map);
    
	
	
	//~ boost_lib::print_graph(G);
	
	std::vector<int> distmap(num_nodes);
	std::vector<boost_lib::Vertex> predmap(num_nodes);
	
	int s = 0;
	
	// Record start time
	double wallTime = 0.0;
    double clockTime = 0.0;
    for (int i=0;i<numRuns;i++)
    {
		auto start = std::chrono::high_resolution_clock::now(); 
		double startClock = std::clock(); 
		
		boost::dijkstra_shortest_paths(G, s, 
				predecessor_map(boost::make_iterator_property_map(
				predmap.begin(), boost::get(boost::vertex_index, G))).
				distance_map(boost::make_iterator_property_map(
				distmap.begin(), boost::get(boost::vertex_index, G))));
		
		//~ print_distances(distmap);
		
		// Record end time
		std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
        wallTime += elapsed.count();
        clockTime += (std::clock() - startClock) / CLOCKS_PER_SEC;

        std::cout << "runtime: " << elapsed.count() << std::endl;
	}

    return 0;
}
