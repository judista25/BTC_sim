# library home:
SIMLIB_DIR=./simlib/src

# dependencies:
SIMLIB_DEPEND=$(SIMLIB_DIR)/simlib.so $(SIMLIB_DIR)/simlib.h
SIMLIB_DEPEND2D=$(SIMLIB_DEPEND) $(SIMLIB_DIR)/simlib2D.h
SIMLIB_DEPEND3D=$(SIMLIB_DEPEND) $(SIMLIB_DIR)/simlib3D.h

main: main.cpp main.hpp $(SIMLIB_DEPEND)
	@echo "Running pre-compilation command..."
	@cd simlib/
	@make
	@cd ..
	$(CXX) $(CXXFLAGS) -o main main.cpp $(SIMLIB_DIR)/simlib.so -lm -Wall -pedantic

.PHONY: clean run

main: main.cpp main.hpp $(SIMLIB_DEPEND)
CXXFLAGS += -I$(SIMLIB_DIR)

% : %.cc
	@#$(CXX) $(CXXFLAGS) -static -o $@  $< $(SIMLIB_DIR)/simlib.a -lm
	$(CXX) $(CXXFLAGS) -o $@  $< $(SIMLIB_DIR)/simlib.so -lm

main: main.cpp $(SIMLIB_DEPEND)
	$(CXX) $(CXXFLAGS) -o main main.cpp $(SIMLIB_DIR)/simlib.so -lm  -Wall -pedantic

clean:
	rm -rf main

run:
	./main

