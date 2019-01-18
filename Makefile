# Set flags
CC=g++
MPICC=mpic++

CPPFLAGS +=-std=c++11 -g -Ofast -march=native
OMPFLAGS = -fopenmp
BOOSTMPIFLAGS +=-std=c++11 -g -O3 -march=native -lboost_mpi -lboost_system -lboost_serialization -lboost_graph_parallel
#LDFLAGS += -Llibmongoose
#LDLIBS += $$(pkg-config vips --libs glib-2.0) -lm

# Set dirs
SEQ_DIR=Sequential
OMP_DIR=SharedMemory
BOOST_DIR=Boost
BOOST_PAR_DIR=BoostParallel

OBJ_DIR=obj
BIN_DIR=bin
#INC_DIR=include
#SRC_DIR=src

# Set objects
all : main 

main : $(BIN_DIR)/runSeq $(BIN_DIR)/runOMP $(BIN_DIR)/testOMP $(BIN_DIR)/benchOMP $(BIN_DIR)/runBoostDijkstra

SEQOBJS : $(SEQ_DIR)/$(OBJ_DIR)/DeltaSteppingSeq.o $(SEQ_DIR)/$(OBJ_DIR)/graph.o
OMPOBJS : $(OMP_DIR)/$(OBJ_DIR)/DeltaStepping.o $(OMP_DIR)/$(OBJ_DIR)/graph.o
BOOSTOBJTS: $(BOOST_DIR)/$(OBJ_DIR)/graph_boost.o

# Set build rules for run files
$(BIN_DIR)/runSeq: $(SEQ_DIR)/runSeq.cpp SEQOBJS
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $< $(SEQ_DIR)/$(OBJ_DIR)/*.o -o $@ 

$(BIN_DIR)/testSeq: $(SEQ_DIR)/testSeq.cpp SEQOBJS
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $< $(SEQ_DIR)/$(OBJ_DIR)/*.o -o $@ 

$(BIN_DIR)/runOMP: $(OMP_DIR)/runOMP.cpp OMPOBJS
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(OMPFLAGS) $< $(OMP_DIR)/$(OBJ_DIR)/*.o -o $@ 

$(BIN_DIR)/benchOMP: $(OMP_DIR)/benchOMP.cpp OMPOBJS
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(OMPFLAGS) $< $(OMP_DIR)/$(OBJ_DIR)/*.o -o $@ 

$(BIN_DIR)/testOMP: $(OMP_DIR)/testOMP.cpp OMPOBJS
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(OMPFLAGS) $< $(OMP_DIR)/$(OBJ_DIR)/*.o -o $@ 

$(BIN_DIR)/runBoostDijkstra: $(BOOST_DIR)/BoostDijkstra.cpp BOOSTOBJTS
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $< $(BOOST_DIR)/$(OBJ_DIR)/*.o -o $@ 
	
$(BIN_DIR)/runBoostDeltaStep:
	$(MPICC) $(BOOST_PAR_DIR)/BoostDeltaStepping.cpp $(BOOSTMPIFLAGS) -o $@

# set buildrules for objects
$(SEQ_DIR)/$(OBJ_DIR)/%.o: $(SEQ_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $< -o $@ 
 
$(OMP_DIR)/$(OBJ_DIR)/%.o: $(OMP_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(OMPFLAGS) -c $< -o $@ 

$(BOOST_DIR)/$(OBJ_DIR)/%.o: $(BOOST_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $< -o $@ 

# Set clean
clean ::
	rm -f $(SEQ_DIR)/$(OBJ_DIR)/* && rm -f $(OMP_DIR)/$(OBJ_DIR)/* && rm -f $(BOOST_DIR)/$(OBJ_DIR)/* && rm -f $(BIN_DIR)/*
