#pragma once
#include "mips/alu.h"
#include "mips/control.h"
#include "mips/decoder.h"
#include "mips/instruction_memory.h"
#include "mips/program_counter.h"
#include "mips/registers.h"
#include "mips/assembler.h"
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
    mips::ControlSignals control;
};

// A single-cycle R-type execution slice, not a pipelined or complete CPU.
// Owns instruction memory and a separate program-counter component.
class SingleCycleCPU {
public:
    SingleCycleCPU();
    RegisterFile registers;
    InstructionMemory instructions;
    uint32_t pc() const;
    void loadProgram(const std::vector<uint32_t>& words,
                     uint32_t base = InstructionMemory::kTextBase);
    void loadProgramFile(const std::string& path,
                         uint32_t base = InstructionMemory::kTextBase);
    bool halted() const; // PC has reached the first address after the program
    ExecutionTrace step(); // fetch from instruction memory, then execute
    std::vector<ExecutionTrace> run(std::size_t maxInstructions = 1000);
    ExecutionTrace stepRType(uint32_t instruction); // legacy direct-word API

private:
    // Assigned to: Zunain Nazir.
    mips::ProgramCounter programCounter_;
    mips::ControlUnit control_;
};

const char* aluOpName(ALUOp op);
std::string formatRType(const DecodedRType& d, ALUOp op);
std::string formatTrace(const ExecutionTrace& trace);
