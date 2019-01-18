#include <fstream>
#include <string>

#include "DeltaSteppingSeq.h"

int main()
{
    std::cout << "HI!"<< std::endl;

    std::string fileName("../graphs/USA-road-d.NY.gr");
    Graph g = g.readFromGR(fileName);

    // 1 source
    const int source = 0;
    const int delta  = 5000;

    DeltaStepping::Dist * dists =
    DeltaStepping::runDeltaStep(g,source,delta);

    FILE *gFile;
	gFile = fopen("../graphs/ny_1.res", "r");
 	if (gFile == NULL) {
   		fprintf(stderr, "ERROR: file %s not found\n", fileName.c_str());
   		exit(1);
	}

    char    in_line[100];
    long    readValue;
    bool    allPassed=true;
    int     count=0, notPassed=0;

    while (fgets(in_line, 100, gFile) != NULL)
  	{
        sscanf ( in_line,"%ld", &readValue);
        bool passed = (int)readValue==dists[count];

        if (!passed)
        {
            std::cout << "node: " << count << " , " << readValue << "!=" << dists[count] << std::endl;
            notPassed++;
        }
        allPassed = allPassed && passed;
        count++;
	}
    fclose(gFile);

    std::cout <<  notPassed << " of " << count << " did not pass" << std::endl;

    /*
    dists = deltaStepper.run(10000-1);

	gFile = fopen("../graphs/ny_10000.res", "r");
 	if (gFile == NULL) {
   		fprintf(stderr, "ERROR: file %s not found\n", fileName.c_str());
   		exit(1);
	}

    count=0;
    notPassed=0;

    while (fgets(in_line, 100, gFile) != NULL)
  	{
        sscanf ( in_line,"%ld", &readValue);
        bool passed = (int)readValue==dists[count];

        if (!passed)
        {
            std::cout << "node: " << count << " , " << readValue << "!=" << dists[count] << std::endl;
            notPassed++;
        }
            
        allPassed = allPassed && passed;
        count++;
	}

    std::cout <<  notPassed << " of " << count << " did not pass" << std::endl;
    std::cout << "Passed fist: " << allPassed << std::endl;
    fclose(gFile);
    */


    
    

    return 0;
}
