CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude
CORE := src/registers.cpp src/alu.cpp src/decoder.cpp src/cpu.cpp src/instruction_memory.cpp
HEADERS := include/registers.hpp include/alu.hpp include/decoder.hpp include/cpu.hpp include/instruction_memory.hpp

.PHONY: all run test clean rebuild
all: demo

demo: src/main.cpp $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) src/main.cpp $(CORE) -o $@

test_core: tests/test_core.cpp $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) tests/test_core.cpp $(CORE) -o $@

test_integration: tests/test_integration.cpp $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) tests/test_integration.cpp $(CORE) -o $@

test_program: tests/test_program.cpp $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) tests/test_program.cpp $(CORE) -o $@

run: demo
	./demo

test: test_core test_integration test_program
	./test_core
	./test_integration
	./test_program

clean:
	rm -f demo test_core test_integration test_program

rebuild: clean all test
