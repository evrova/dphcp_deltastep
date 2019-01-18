#include "graph_boost_par.hpp"

int main(int argc, char * argv[]){
	
	boost::mpi::environment env(argc, argv);
	
	const char* filename("/home/evrova/MASTERS/DPHPC/DPHCP_Project/dpchp/graphs/tiny_02.graph");

	std::ifstream in(filename);
	boost::graph::metis_reader reader(in);
	
	boost_lib_par::DistrGraph G(reader.begin(), reader.end(), reader.weight_begin(), reader.num_vertices());

	boost_lib_par::Vertex s = vertex(0, G);

	double start = MPI_Wtime();
	
	graph::distributed::delta_stepping_shortest_paths(G, s, dummy_property_map(),
		get(vertex_distance, G), get(edge_weight, G));
		
	double end = MPI_Wtime();
	
	if (process_id(process_group(G)) == 0) {
		std::cout << "Elapsed time: " <<end-start<<std::endl;
	}

	return 0;
}
