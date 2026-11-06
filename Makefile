CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude
CORE := src/registers.cpp src/alu.cpp
.PHONY: all test clean
all: demo
demo: src/main.cpp $(CORE)
	$(CXX) $(CXXFLAGS) $^ -o $@
test_core: tests/test_core.cpp $(CORE)
	$(CXX) $(CXXFLAGS) $^ -o $@
test: test_core
	./test_core
clean:
	rm -f demo test_core
