SIMLIB_DIR=./simlib/src

SIMLIB_DEPEND=$(SIMLIB_DIR)/simlib.so $(SIMLIB_DIR)/simlib.h
SIMLIB_DEPEND2D=$(SIMLIB_DEPEND) $(SIMLIB_DIR)/simlib2D.h
SIMLIB_DEPEND3D=$(SIMLIB_DEPEND) $(SIMLIB_DIR)/simlib3D.h


CXXFLAGS += -I$(SIMLIB_DIR)

.PHONY: clean run all


all: main
	make -C simlib install
	make -C .


main: main.cpp main.hpp 
	$(CXX) $(CXXFLAGS) -o main main.cpp $(SIMLIB_DIR)/simlib.so -lm -Wall -pedantic

# $(SIMLIB_DIR)/simlib.so:
# 	@echo "Building simlib.so..."
	

clean:
	rm -rf main

run: main
	./main