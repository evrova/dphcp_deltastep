# Makefile-based Benchmarking Infrastructure
# Scott Beamer, 2017



# Generate Input Graphs ------------------------------------------------#
#-----------------------------------------------------------------------#

GRAPH_DIR = benchmark/graphs
RAW_GRAPH_DIR = benchmark/graphs/raw

GRAPHS = road #kron twitter road urand web
ALL_GRAPHS =$(addsuffix .wsg, $(GRAPHS))
ALL_GRAPHS_WITH_PATHS = $(addprefix $(GRAPH_DIR)/, $(ALL_GRAPHS))

$(RAW_GRAPH_DIR):
	mkdir -p $@

.PHONY: bench-graphs
bench-graphs: $(RAW_GRAPH_DIR) $(ALL_GRAPHS_WITH_PATHS)

# Real-world

TWITTER_URL = http://an.kaist.ac.kr/~haewoon/release/twitter_social_graph/twitter_rv.tar.gz
$(RAW_GRAPH_DIR)/twitter_rv.tar.gz:
	wget -P $(RAW_GRAPH_DIR) $(TWITTER_URL)

$(RAW_GRAPH_DIR)/twitter_rv.net: $(RAW_GRAPH_DIR)/twitter_rv.tar.gz
	tar -zxvf $< -C $(RAW_GRAPH_DIR)
	touch $@

$(RAW_GRAPH_DIR)/twitter.el: $(RAW_GRAPH_DIR)/twitter_rv.net
	rm -f $@
	ln -s twitter_rv.net $@$@

$(GRAPH_DIR)/twitter.wsg: $(RAW_GRAPH_DIR)/twitter.el converter
	./converter -sf $< -wb $@

ROAD_URL = http://www.dis.uniroma1.it/challenge9/data/USA-road-d/USA-road-d.USA.gr.gz
$(RAW_GRAPH_DIR)/USA-road-d.USA.gr.gz:
	wget -P $(RAW_GRAPH_DIR) $(ROAD_URL)

$(RAW_GRAPH_DIR)/USA-road-d.USA.gr: $(RAW_GRAPH_DIR)/USA-road-d.USA.gr.gz
	cd $(RAW_GRAPH_DIR)
	gunzip < $< > $@

$(GRAPH_DIR)/road.wsg: $(RAW_GRAPH_DIR)/USA-road-d.USA.gr converter
	./converter -sf $< -wb $@

WEB_URL = https://www.cise.ufl.edu/research/sparse/MM/LAW/sk-2005.tar.gz
$(RAW_GRAPH_DIR)/sk-2005.tar.gz:
	wget -P $(RAW_GRAPH_DIR) $(WEB_URL)

$(RAW_GRAPH_DIR)/sk-2005/sk-2005.mtx: $(RAW_GRAPH_DIR)/sk-2005.tar.gz
	tar -zxvf $< -C $(RAW_GRAPH_DIR)
	touch $@

$(GRAPH_DIR)/web.wsg: $(RAW_GRAPH_DIR)/sk-2005/sk-2005.mtx converter
	./converter -sf $< -wb $@

# Synthetic

KRON_ARGS = -g18 -k16

$(GRAPH_DIR)/kron.wsg: converter
	./converter $(KRON_ARGS) -s -wb $@

URAND_ARGS = -u18 -k16

$(GRAPH_DIR)/urand.wsg: converter
	./converter $(URAND_ARGS) -s -wb $@


# Benchmark Execution --------------------------------------------------#
#-----------------------------------------------------------------------#

OUTPUT_DIR = benchmark/out

$(OUTPUT_DIR):
	mkdir -p $@

# Ordered to reuse input graphs to increase OS file cache hit probability
BENCH_ORDER = \
	sssp-$(GRAPH_NAME) delt-$(GRAPH_NAME) #sssp-urand delt-urand sssp-web delt-web sssp-twitter delt-twitter 

OUTPUT_FILES = $(addsuffix -$(OMP_NUM_THREADS).out, $(addprefix $(OUTPUT_DIR)/, $(BENCH_ORDER)))

.PHONY: bench-run
bench-run: $(OUTPUT_DIR) $(OUTPUT_FILES)

SSSP_ARGS = -n64
DELT_ARGS = -n 64


$(OUTPUT_DIR)/sssp-web-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/web.wsg sssp
	./sssp -f $< $(SSSP_ARGS) -d2 > $@
	sed -i -n '/Time/p' $@

$(OUTPUT_DIR)/delt-web-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/web.wsg
	../bin/benchOMP -f $< $(DELT_ARGS) -d 2 > $@

$(OUTPUT_DIR)/sssp-road-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/road.wsg sssp
	./sssp -f $< $(SSSP_ARGS) -d50000 > $@
	sed -i -n '/Time/p' $@

$(OUTPUT_DIR)/delt-road-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/road.wsg
	../bin/benchOMP -f $< $(DELT_ARGS) -d 50000 > $@

$(OUTPUT_DIR)/sssp-kron-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/kron.wsg sssp
	./sssp -f $< $(SSSP_ARGS) -d2 > $@
	sed -i -n '/Time/p' $@

$(OUTPUT_DIR)/delt-kron-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/kron.wsg
	../bin/benchOMP -f $< $(DELT_ARGS) -d 2 > $@

$(OUTPUT_DIR)/sssp-twitter-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/twitter.wsg sssp
	./sssp -f $< $(SSSP_ARGS) -d2 > $@
	sed -i -n '/Time/p' $@

$(OUTPUT_DIR)/delt-twitter-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/twitter.wsg
	../bin/benchOMP -f $< $(DELT_ARGS) -d 2 > $@

$(OUTPUT_DIR)/sssp-urand-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/urand.wsg sssp
	./sssp -f $< $(SSSP_ARGS) -d2 > $@
	sed -i -n '/Time/p' $@

$(OUTPUT_DIR)/delt-urand-$(OMP_NUM_THREADS).out: $(GRAPH_DIR)/urand.wsg
	../bin/benchOMP -f $< $(DELT_ARGS) -d 2 > $@


