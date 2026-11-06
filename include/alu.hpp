#pragma once
#include <cstdint>

class RegisterFile;

// Single-cycle ALU operations; MULT/DIV, HI/LO, memory and branches are out of scope.
enum class ALUOp { ADD, ADDU, SUB, SUBU, AND, OR, XOR, NOR, SLT, SLTU, SLL, SRL, SRA };

struct ALUResult {
    uint32_t value;
    bool zero;
};

// shamt applies only to SLL/SRL/SRA. For shifts, rhs supplies the value from rt.
ALUResult aluEvaluate(ALUOp op, uint32_t lhs, uint32_t rhs, unsigned shamt = 0);
// Backward-compatible API from previous starter.
uint32_t aluExecute(ALUOp op, uint32_t lhs, uint32_t rhs);
// R-type funct field -> operation. Throws on unsupported function codes.
ALUOp aluOpFromFunct(unsigned funct);
// Single-cycle register -> ALU -> write-back, with optional shift amount.
// Not a complete CPU or assembly interpreter. Throws before writing on failure.
void executeRType(RegisterFile& regs, ALUOp op,
                  unsigned rd, unsigned rs, unsigned rt, unsigned shamt = 0);
