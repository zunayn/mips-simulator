#pragma once
#include "alu.hpp"
#include "decoder.hpp"
#include "instruction_memory.hpp"
#include "registers.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <cstddef>

struct ExecutionTrace {
    uint32_t pcBefore, pcAfter;
    DecodedRType decoded;
    ALUOp operation;
    uint32_t operandA, operandB, result;
    bool zero, regWrite;
    std::string assembly;
};

// A single-cycle R-type execution slice, not a pipelined or complete CPU.
// A caller supplies one instruction word (instruction memory is not yet modeled).
class SingleCycleCPU {
public:
    RegisterFile registers;
    InstructionMemory instructions;
    uint32_t pc = InstructionMemory::kTextBase;
    void loadProgram(const std::vector<uint32_t>& words,
                     uint32_t base = InstructionMemory::kTextBase);
    void loadProgramFile(const std::string& path,
                         uint32_t base = InstructionMemory::kTextBase);
    bool halted() const; // PC has reached the first address after the program
    ExecutionTrace step(); // fetch from instruction memory, then execute
    std::vector<ExecutionTrace> run(std::size_t maxInstructions = 1000);
    ExecutionTrace stepRType(uint32_t instruction); // legacy direct-word API
};

const char* aluOpName(ALUOp op);
std::string formatRType(const DecodedRType& d, ALUOp op);
std::string formatTrace(const ExecutionTrace& trace);
