#include "graph.h"
#include "DeltaStepping.h"

#include <chrono>
#include <random>

int main(int argc, char** argv)
{
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

    Graph g = Graph::readFromWSG(fileName);

    static const int64_t kRandSeed = 27491095;
    std::mt19937 rng(kRandSeed);    // random-number engine used (Mersenne-Twister in this case) 
    std::uniform_int_distribution<Graph::Node> uni(0,g.getNumberOfNodes()-1); // guaranteed unbiased 

    for (int i=0;i<numRuns;i++)
    {
        int source = uni(rng);

        auto start = std::chrono::high_resolution_clock::now(); 
        double startClock = std::clock(); 

        DeltaStepping::Dist * res =
        DeltaStepping::runDeltaStep(g,source,delta);

        std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
        std::cout << elapsed.count() << "\t" << (std::clock() - startClock) / CLOCKS_PER_SEC << std::endl;

        delete[] res;
    }
    return 0;
}
