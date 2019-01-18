#ifndef GRAPH_BOOST_H
#define GRAPH_BOOST_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace boost_lib{

	// Graph Type
	// OutEdgeList Type (for each vertex, adjacency list is kept in a vector)
	//, VertexList Type (a list of all edges is kept in a vector), (un)directedS
	// nested vertex properties
	// nested edge properties
	// vecS = for each vertex, adjacency list kept in a vector, choosing setS disallows for parallel edges
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
			boost::no_property, boost::property<boost::edge_weight_t, int>> Graph;

	typedef boost::graph_traits<Graph>::vertex_descriptor Vertex; // Vertex type
	typedef boost::graph_traits<Graph>::edge_descriptor Edge; // Edge type
	typedef boost::graph_traits<Graph>::edge_iterator EdgeIt; //Edge iterator
	// Property map edge -> weight 
	typedef boost::property_map<Graph, boost::edge_weight_t>::type WeightMap;

	void add_edge(Graph &G, WeightMap &weight_map, int u, int v, int w);
	long readFromGR(const std::string & fileName, Graph &G, WeightMap &weight_map);	
	void print_graph(boost_lib::Graph &G);

}
#endif
