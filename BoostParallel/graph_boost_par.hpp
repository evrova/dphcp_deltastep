#ifndef GRAPH_BOOST_PAR_H
#define GRAPH_BOOST_PAR_H

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/delta_stepping_shortest_paths.hpp>
#include <boost/graph/metis.hpp>
#include <boost/graph/distributed/graphviz.hpp>

#include <fstream>
#include <string>

using namespace boost;
using boost::graph::distributed::mpi_process_group;

namespace boost_lib_par{

	typedef boost::adjacency_list<boost::vecS, distributedS<mpi_process_group, boost::vecS>, undirectedS,
		boost::property<vertex_distance_t, float>, boost::property<edge_weight_t, float> > DistrGraph;

	typedef boost::graph_traits<DistrGraph>::vertex_descriptor Vertex;
	
	//~ long readFromGR(const std::string & fileName, Graph &G, WeightMap &weight_map);	

}
#endif
