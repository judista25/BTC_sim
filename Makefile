# Linux/Unix makefile for basic SIMLIB examples
# expects simlib.so in SIMLIB_DIR

# library home:
SIMLIB_DIR=./simlib/src

# dependencies:
SIMLIB_DEPEND=$(SIMLIB_DIR)/simlib.so $(SIMLIB_DIR)/simlib.h
SIMLIB_DEPEND2D=$(SIMLIB_DEPEND) $(SIMLIB_DIR)/simlib2D.h
SIMLIB_DEPEND3D=$(SIMLIB_DEPEND) $(SIMLIB_DIR)/simlib3D.h

# add flags:
main: main.cpp main.hpp $(SIMLIB_DEPEND)
CXXFLAGS += -I$(SIMLIB_DIR)

# Implicit rule to compile modules
% : %.cc
	@#$(CXX) $(CXXFLAGS) -static -o $@  $< $(SIMLIB_DIR)/simlib.a -lm
	$(CXX) $(CXXFLAGS) -o $@  $< $(SIMLIB_DIR)/simlib.so -lm

# Main program
main: main.cpp $(SIMLIB_DEPEND)
	$(CXX) $(CXXFLAGS) -o main main.cpp $(SIMLIB_DIR)/simlib.so -lm

plot: run
	@./0-plot-all-ps.sh

clean:
	@echo "Deleting all executable examples..."
	rm -f $(ALL_MODELS) gmon.out

clean-data:
	@echo "Deleting all simulation results..."
	rm -f *.dat *.out *.png *.ps *.pdf reference-outputs/*.log

clean-all: clean clean-data

REFOUT=reference-outputs/*.dat reference-outputs/*.out reference-outputs/*.sh

pack:
	tar czf examples.tar.gz *.cc *.plt *.txt *.sh Makefile.* $(REFOUT)

#
