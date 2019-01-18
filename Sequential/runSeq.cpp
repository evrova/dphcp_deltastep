#include "graph.h"
#include "DeltaSteppingSeq.h"

#include <chrono>

int main()
{
    std::cout << "HI!"<< std::endl;

    //std::string fileName("../graphs/sample.gr");
    std::string fileName("../graphs/USA-road-d.NY.gr");
    Graph g = g.readFromGR(fileName);
    //g.print_graph();

    int numRuns = 10;

    double time = 0.0;
    for (int i=0;i<numRuns;i++)
    {
        double t1 = std::clock();
        DeltaStepping deltaStepper(g, 5000);
        deltaStepper.run(0);
        time += (std::clock() - t1) / CLOCKS_PER_SEC;
    }
    time /= (double)numRuns;

    std::cout << "average runtime: " << time << std::endl; 
    
    

    return 0;
}