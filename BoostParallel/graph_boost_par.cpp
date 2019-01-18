#include "graph_boost_par.hpp"

//~ void boost_lib::print_graph(boost_lib::Graph &G){

	
//~ }



//~ long boost_lib::readFromGR(const std::string & fileName, boost_lib::Graph &G, boost_lib::WeightMap &weight_map)
//~ {
	//~ //reads .gr files. see http://www.dis.uniroma1.it/challenge9/format.shtml#graph
	//~ FILE *gFile;
	//~ gFile = fopen(fileName.c_str(), "r");
 	//~ if (gFile == NULL) {
   		//~ fprintf(stderr, "ERROR: file %s not found\n", fileName.c_str());
   		//~ exit(1);
	//~ }

	//~ char    in_line[100],pr_type[3];

    //~ long numberOfNodes, numberOfArcs;

	//~ // go through file looking for problem description
	//~ bool foundHead = false;
	//~ while (fgets(in_line, 100, gFile) != NULL && !foundHead)
  	//~ {
		//~ switch (in_line[0])
		//~ {
			//~ case 'p':
				//~ sscanf ( in_line, "%*c %2s %ld %ld", pr_type, &numberOfNodes, &numberOfArcs);
				//~ std::cout << "Problem type: " << pr_type <<std::endl;
				//~ std::cout << "Number of nodes: " << numberOfNodes <<std::endl;
				//~ std::cout << "Number of Arcs: " << numberOfArcs <<std::endl;
				//~ foundHead = true;
				//~ break;

			//~ default:
				//~ break;
		//~ }
	//~ }
	//~ rewind(gFile);

	//~ long head,tail,weight;
		
	//~ while (fgets(in_line, 100, gFile) != NULL)
  	//~ {
		//~ switch (in_line[0])
		//~ {
			//~ case 'a':
				//~ sscanf ( in_line,"%*c %ld %ld %ld",&tail, &head, &weight);
				//~ tail -= 1;
				//~ head -= 1;

				//~ boost_lib::add_edge(G, weight_map, head, tail, weight);
				//~ break;

			//~ default:
				//~ break;
		//~ }
	//~ }
	
	//~ fclose(gFile);
	
	//~ return numberOfNodes;

//~ }
