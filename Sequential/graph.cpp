#include "graph.h"

// code from stack overflow
void Graph::print_graph() 
{ 
    
    std::cout<<"------------------------------------------[GRAPH]----------------------------------------------"<<std::endl;
    std::cout << "Number of nodes: " << numberOfNodes << std::endl;
	std::cout << "Number of arcs : " << numberOfArcs << std::endl;
	for (int i = 0; i < numberOfNodes; ++i) 
    {
        std::cout<<"[Node " <<i<<"]"<<std::endl; 
        for (int j = numberOfNeighbours[i]; j < numberOfNeighbours[i+1]; ++j) 
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
Graph::getAdjecencyPtr(){
	return this->_adjacency;
}

int *
Graph::getNeighbourPtr(){
	return this->numberOfNeighbours;
}



// todo: write this together with finding depth of graph
int
Graph::getMaxEdgeWeight() const
{
    // this is very crude, and should only be called once
    int maximalEdge = 0;

    for (int i = 0; i < 2*numberOfArcs; ++i)
    {
            if (maximalEdge < _adjacency[i].second)
                maximalEdge = _adjacency[i].second;
    }

    return maximalEdge;
}

int
Graph::getMaxDegree() const
{
	
	std::cout<<"ENTERS NEW!!!!!!!!"<<std::endl;
	int maxDegree = 0;
	int currDegree = 0;
	
    for (int i = 1; i < numberOfNodes; ++i)
    {
		currDegree = numberOfNeighbours[i] - numberOfNeighbours[i-1];
		
		if(maxDegree < currDegree){
			maxDegree = currDegree;
		}
    }
    return maxDegree;
}

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

    Edge * adjencencyArray 	= new Edge[2*numberOfArcs];
	int * numAdjecentArray 	= new int[numberOfNodes+1];

    for (long i=0;i<numberOfNodes;i++)
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
