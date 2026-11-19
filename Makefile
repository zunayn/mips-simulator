CXX ?= c++
CPPFLAGS += -Iinclude
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CORE := src/registers.cpp src/alu.cpp src/decoder.cpp src/cpu.cpp src/instruction_memory.cpp src/program_counter.cpp
HEADERS := include/mips/registers.h include/mips/alu.h include/mips/decoder.h include/mips/cpu.h include/mips/instruction_memory.h include/mips/program_counter.h
CORE += src/instruction.cpp src/control.cpp src/assembler.cpp
HEADERS += include/mips/instruction.h include/mips/control.h include/mips/assembler.h

.PHONY: all run test clean rebuild
all: demo

demo: src/main.cpp $(CORE) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/main.cpp $(CORE) -o $@

test_core: tests/test_core.cpp $(CORE) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_core.cpp $(CORE) -o $@

test_integration: tests/test_integration.cpp $(CORE) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_integration.cpp $(CORE) -o $@

test_program: tests/test_program.cpp $(CORE) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_program.cpp $(CORE) -o $@

test_program_counter: tests/test_program_counter.cpp src/program_counter.cpp include/mips/program_counter.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_program_counter.cpp src/program_counter.cpp -o $@

test_control: tests/test_control.cpp $(CORE) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_control.cpp $(CORE) -o $@

run: demo
	./demo

test: test_core test_integration test_program test_program_counter test_control
	./test_core
	./test_integration
	./test_program
	./test_program_counter
	./test_control

clean:
	rm -f demo test_core test_integration test_program test_program_counter test_control

rebuild: clean all test
