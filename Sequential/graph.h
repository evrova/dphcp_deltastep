/**
 * @file graph.h
 * @brief Main header file for the graph library
 * @author 
 * @date 10/2018
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>
#include <list>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>
		
class Graph
{
	public:
		typedef int Node;
		typedef int Weight;
		typedef std::pair<Node, Weight> Edge;
		
	Graph(){}
		
	/*
	Graph(const std::vector<std::vector<Edge>> & adjacency) : _adjacency(adjacency){
		_adjacency.shrink_to_fit();
	}
	*/

	Graph(long numberOfNodes, long numberOfArcs, int * numberOfNeighbours, Edge * adjacency)
	: numberOfNodes(numberOfNodes), numberOfArcs(numberOfArcs), numberOfNeighbours(numberOfNeighbours), _adjacency(adjacency){}
	

	~Graph()
	{
		delete[] _adjacency;
		delete[] numberOfNeighbours;
	}
	
	void print_graph();

	//static Graph readFromRMAT(const std::string & fileName);

	static Graph readFromGR(const std::string & fileName);

	/*
	std::vector<Edge>
	getNeighbours(const Node & node) const;

	std::vector<Edge> *
	getNeighbourReference(const Node & node);
	*/

	long
	getNumberOfNodes() const;

	long
	getNumberOfArcs() const;

	Edge *
	getAdjecencyPtr();

	int *
	getNeighbourPtr();

	int
	getMaxEdgeWeight() const;
	
	int
	getMaxDegree() const;

	private:
		//std::vector<std::vector<Edge>> _adjacency;
		
		long numberOfNodes;
		long numberOfArcs;

		int * numberOfNeighbours;
		Edge * _adjacency; // metis format
};

#endif
