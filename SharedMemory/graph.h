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
		typedef int32_t Node;
		typedef int32_t Weight;
		typedef int64_t NodeOffset;
		typedef std::pair<Node, Weight> Edge;
		
	Graph(){}
		
	/*
	Graph(const std::vector<std::vector<Edge>> & adjacency) : _adjacency(adjacency){
		_adjacency.shrink_to_fit();
	}
	*/

	Graph(long numberOfNodes, long numberOfArcs, NodeOffset * numberOfNeighbours, Edge * adjacency)
	: numberOfNodes(numberOfNodes), numberOfArcs(numberOfArcs), numberOfNeighbours(numberOfNeighbours), _adjacency(adjacency){}
	
	~Graph()
	{
		delete[] _adjacency;
		delete[] numberOfNeighbours;
	}
	
	void print_graph();

	//static Graph readFromRMAT(const std::string & fileName);

	static Graph 
	readFromGR(const std::string & fileName);

	static Graph
	readFromWSG(const std::string & fileName);

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
	getAdjecencyPtr() const;

	NodeOffset *
	getNeighbourPtr() const;

	Weight
	getMaxEdgeWeight() const;

	private:
		//std::vector<std::vector<Edge>> _adjacency;
		
		long numberOfNodes = 0;
		long numberOfArcs = 0;

		NodeOffset * numberOfNeighbours = nullptr;
		Edge * _adjacency = nullptr; // metis format
};

#endif
