#include "graph.h"
#include "DeltaStepping.h"

#include <chrono>
#include <random>

int main(int argc, char** argv)
{
    std::cout << "HI!"<< std::endl;
    std::string fileName("../graphs/ny.wsg");
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

    //std::string fileName("../graphs/sample.gr");
    
    Graph g = Graph::readFromWSG(fileName);
    //g.print_graph();
    //Graph g = Graph::readFromGR(fileName);

    std::cout << "Done reading graph" << std::endl;
    std::cout << "number of nodes: " << g.getNumberOfNodes() << std::endl;
    std::cout << "number of edges: " << g.getNumberOfArcs() << std::endl;

    //g.print_graph();

    static const int64_t kRandSeed = 27491095;
    std::mt19937 rng(kRandSeed);    // random-number engine used (Mersenne-Twister in this case) 
    std::uniform_int_distribution<Graph::Node> uni(0,g.getNumberOfNodes()-1); // guaranteed unbiased 
    //g.print_graph();

    double wallTime = 0.0;
    double clockTime = 0.0;
    for (int i=0;i<numRuns;i++)
    {
        const Graph::Node source = uni(rng);
        //std::cout << "source: " << source << std::endl;

        auto start = std::chrono::high_resolution_clock::now(); 
        double startClock = std::clock(); 

        DeltaStepping::Dist * res =
        DeltaStepping::runDeltaStep(g,source,delta);

        std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
        wallTime += elapsed.count();
        clockTime += (std::clock() - startClock) / CLOCKS_PER_SEC;

        std::cout << "runtime: " << elapsed.count() << std::endl;
        //~ std::cout << "cpu time: " << (std::clock() - startClock) / CLOCKS_PER_SEC  << std::endl  << std::endl;

        delete[] res;
    }

    wallTime /= (double)numRuns;
    clockTime /= (double)numRuns;

    std::cout << "average runtime: " << wallTime << std::endl; 
    //~ std::cout << "average clock time: " << clockTime << std::endl; 
    
    return 0;
}
