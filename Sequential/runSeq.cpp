#include "DeltaSteppingSeq.h"
#include <chrono>

#include <chrono>

int main(int argc, char** argv)
{
    std::cout << "HI!"<< std::endl;
    std::string fileName("../graphs/USA-road-d.NY.gr");
    int delta = -1;
    int numRuns = 10;

    for (int i = 1; i < argc; i++) {
        std::string option(argv[i]);
        if (option==std::string("-f")) {
            fileName.clear();
            fileName = argv[i + 1];
        } else if (option==std::string("-d")) {
            delta = atoi(argv[i + 1]);
        } else if (option==std::string("-n")) {
            numRuns = atoi(argv[i + 1]);
        }
    }

	Graph g = g.readFromGR(fileName);
    std::cout<<"Graph loaded from:"<<fileName.c_str()<<std::endl;
    
    
    //g.print_graph();
    int num_nodes = g.getNumberOfNodes();
    
    // create tenative distances vector where the result will be saved:
	DeltaStepping::Dist * tent_dists = new DeltaStepping::Dist[num_nodes];
	int source = 0;

    double time = 0.0;
    for (int i = 0; i < numRuns; ++i)
    {
        double t1 = std::clock();
        DeltaStepping::runDeltaStep(g, source, delta, tent_dists);
        time += (std::clock() - t1) / CLOCKS_PER_SEC;
    }
    
    time /= (double)numRuns;

    std::cout << "average runtime: " << time << std::endl; 
    
    delete[] tent_dists; // de allocate the memory
    

    return 0;
}
