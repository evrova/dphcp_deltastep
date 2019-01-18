#include "graph.h"

void Graph::print_graph() 
{ 
    
    std::cout<<"------------------------------------------[GRAPH]----------------------------------------------"<<std::endl;
    std::cout << "Number of nodes: " << numberOfNodes << std::endl;
	std::cout << "Number of arcs : " << numberOfArcs << std::endl;
	for (int i = 0; i < numberOfNodes; ++i) 
    {
        std::cout<<"[Node " <<i<<"]"<<std::endl; 
        for (Node j = numberOfNeighbours[i]; j < numberOfNeighbours[i+1]; ++j) 
        { 
            std::cout<<"\tNode "<<_adjacency[j].first <<": weight "<< _adjacency[j].second<<"\n"; 
        } 
        std::cout<<std::endl; 
    } 
} 

long
Graph::getNumberOfNodes() const
{
    return numberOfNodes;
};

long
Graph::getNumberOfArcs() const
{
    return numberOfArcs;
};

Graph::Edge *
Graph::getAdjecencyPtr() const{
	return this->_adjacency;
}

Graph::NodeOffset *
Graph::getNeighbourPtr() const{
	return this->numberOfNeighbours;
}


// todo: write this together with finding depth of graph
Graph::Weight
Graph::getMaxEdgeWeight() const
{
    // this is very crude, and should only be called once
    int maximalEdge = 0;

	#pragma omp parallel for reduction(max:maximalEdge)
    for (int i = 0; i < numberOfArcs; ++i)
    {
            if (maximalEdge < _adjacency[i].second)
                maximalEdge = _adjacency[i].second;
    }

    return maximalEdge;
};

/* todo: fix
Graph
Graph::readFromRMAT(const std::string & fileName)
{
    std::fstream file;
    file.open(fileName.c_str());
	
	std::string line;
    std::vector<std::vector<Edge>> adjacency;

    std::string tmp = "";
    Node node_1 = 0;
    Node node_2 = 0;
    int weight = 0;

    // read number of nodes
	while(!file.eof())
    {
		std::getline(file, line, '\n');
        size_t space_idx = line.find("\t", 0);
        
        if (space_idx != -1)
        {
            // node 1
            tmp = line.substr(0, space_idx);
            node_1 = stoi(tmp);
            std::cout<<node_1<<" ";
            
            // node 2
            tmp = line.substr(space_idx);
            node_2 = stoi(tmp);
            std::cout<<node_2<<std::endl;

            if ((uint)node_1 > adjacency.size())
                adjacency.resize(node_1);

            if ((uint)node_2 > adjacency.size())
                adjacency.resize(node_2);
            
            weight = (rand() % 10) + 1; // random weight between [1, 10]
            
            adjacency[node_1].emplace_back(node_2, weight);
            adjacency[node_2].emplace_back(node_1, weight);
        }
	}
	file.close();

    // unroll vector into arrays
    
    uint numNodes = adjacency.size();
    Edge** adjPointer = new Edge*[numNodes];
    int* numNeigh = new int[numNodes];

    for (int i=0; i<numNodes; i++)
    {
        numNeigh[i] = adjacency[i].size();
        adjPointer[i] = new Edge[numNeigh[i]];
        for (int j=0; j<numNeigh[i]; j++)
            adjPointer[i][j] = adjacency[i][j];
    }
    
    return Graph(adjacency);
};
*/

Graph
Graph::readFromGR(const std::string & fileName)
{
	//reads .gr files. see http://www.dis.uniroma1.it/challenge9/format.shtml#graph
	FILE *gFile;
	gFile = fopen(fileName.c_str(), "r");
 	if (gFile == NULL) {
   		fprintf(stderr, "ERROR: file %s not found\n", fileName.c_str());
   		exit(1);
	}

	char    in_line[100],pr_type[3];

    long numberOfNodes, numberOfArcs;

	// go through file looking for problem description
	bool foundHead = false;
	while (fgets(in_line, 100, gFile) != NULL && !foundHead)
  	{
		switch (in_line[0])
		{
			case 'p':
				sscanf ( in_line, "%*c %2s %ld %ld", pr_type, &numberOfNodes, &numberOfArcs);
				std::cout << "Problem type: " << pr_type <<std::endl;
				std::cout << "Number of nodes: " << numberOfNodes <<std::endl;
				std::cout << "Number of Arcs: " << numberOfArcs <<std::endl;
				foundHead = true;
				break;

			default:
				break;
		}
	}
	rewind(gFile);

    Edge * adjencencyArray 	    = new Edge[2*numberOfArcs];
	NodeOffset * numAdjecentArray 	= new NodeOffset[numberOfNodes+1];

    for (long i=0;i<numberOfNodes+1;i++)
		numAdjecentArray[i] = 0;

    for (long i=0;i<2*numberOfArcs;i++)
		adjencencyArray[i] = Edge(-1,-1);

	long head,tail,weight;
		
    // read number of neighbours
	while (fgets(in_line, 100, gFile) != NULL)
  	{
		switch (in_line[0])
		{
			case 'a':
				sscanf ( in_line,"%*c %ld %ld %ld",&tail, &head, &weight);
				numAdjecentArray[--tail]++;
				numAdjecentArray[--head]++;
				break;

			default:
				break;
		}
	}
	rewind(gFile);

    long sum=0;
	int padding;
	for (long i=0;i<numberOfNodes+1;i++)
	{
		sum += numAdjecentArray[i];
		numAdjecentArray[i] = sum - numAdjecentArray[i];
	}

	while (fgets(in_line, 100, gFile) != NULL)
  	{
		switch (in_line[0])
		{
			case 'a':
				sscanf ( in_line,"%*c %ld %ld %ld",&tail, &head, &weight);
				--tail;
				--head;

                padding = 0;
				while (adjencencyArray[numAdjecentArray[tail]+padding].first != -1)
					padding++;

				adjencencyArray[numAdjecentArray[tail]+padding] = Edge(head,weight);

				padding = 0;
				while (adjencencyArray[numAdjecentArray[head]+padding].first != -1)
					padding++;

				adjencencyArray[numAdjecentArray[head]+padding] = Edge(tail,weight);

				break;
			default:
				break;
		}
	}
	fclose(gFile);

    return Graph(numberOfNodes,numberOfArcs,numAdjecentArray,adjencencyArray);
};

Graph
Graph::readFromWSG(const std::string & fileName)
{
	//reads .wsg files
	std::ifstream file(fileName);
    if (!file.is_open()) {
      std::cout << "Couldn't open file " << fileName << std::endl;
      std::exit(1);
    }

	typedef int32_t DestID_;
	typedef int64_t SGOffset;

    bool directed;
    SGOffset num_nodes, num_edges;
    file.read(reinterpret_cast<char*>(&directed), sizeof(bool));
    file.read(reinterpret_cast<char*>(&num_edges), sizeof(SGOffset));
    file.read(reinterpret_cast<char*>(&num_nodes), sizeof(SGOffset));

    SGOffset * offsets  = new SGOffset[num_nodes+1];
    Edge *  neighs      = new Edge[num_edges];//DestID_[2*num_edges];

    // touch memory by static distribution to allocate across sockets
    #pragma omp parallel
    {
        #pragma omp for nowait schedule(static)
        for (long i=0; i<num_nodes+1; ++i)
            offsets[i] = 0;

        #pragma omp for nowait schedule(static)
        for (long i=0; i<num_edges; ++i)
            neighs[i] = {0,0};
    }

    std::streamsize num_index_bytes = (num_nodes+1) * sizeof(SGOffset);
    std::streamsize num_neigh_bytes = 2 * num_edges * sizeof(DestID_);
    file.read(reinterpret_cast<char*>(offsets), num_index_bytes);
    file.read(reinterpret_cast<char*>(neighs), num_neigh_bytes);
    file.close();

    // POSTPROCESS
    NodeOffset * newOffset = new NodeOffset[num_nodes+1];    
    for (int i=0; i<num_nodes+1; ++i)
        newOffset[i] = (NodeOffset)offsets[i];
    delete[] offsets;

    //Edge * adjecency = new Edge[num_edges];
    //for (int i=0; i<num_edges; ++i)
    //    adjecency[i] = Edge(neighs[2*i] , neighs[2*i+1]);
    //delete[] neighs;

    // hack to fix .gr bugs
    num_nodes++;

    /*
    std::cout << std::endl << "offsets:" << std::endl;
    for (int i=0; i<num_nodes+1;++i)
        std::cout << newOffset[i] << " " << std::endl;

    std::cout << std::endl << "edges:" << std::endl;
    for (int i=0; i<num_edges;++i)
        std::cout << adjecency[i].first << " , " << adjecency[i].second << std::endl;
    */

    // postprocess into something sensible
	
    return Graph(num_nodes-1,num_edges,newOffset,neighs);
};