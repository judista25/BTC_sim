SIMLIB_DIR=./simlib/src

SIMLIB_DEPEND=$(SIMLIB_DIR)/simlib.so $(SIMLIB_DIR)/simlib.h
SIMLIB_DEPEND2D=$(SIMLIB_DEPEND) $(SIMLIB_DIR)/simlib2D.h
SIMLIB_DEPEND3D=$(SIMLIB_DEPEND) $(SIMLIB_DIR)/simlib3D.h

$(SIMLIB_DIR)/simlib.so:
	@echo "Building simlib.so..."
	$(MAKE) -C simlib install

CXXFLAGS += -I$(SIMLIB_DIR)

main: main main.cpp main.hpp $(SIMLIB_DEPEND)
	$(CXX) $(CXXFLAGS) -o main main.cpp $(SIMLIB_DIR)/simlib.so -lm -Wall -pedantic

.PHONY: clean run all

all: main

clean:
	rm -rf main

run: main
	./main