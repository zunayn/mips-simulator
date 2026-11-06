#pragma once
#include <cstdint>

enum class ALUOp { ADD, SUB, AND, OR, SLT };

// Signed comparison (SLT); ADD/SUB throw on signed overflow as MIPS add/sub do.
uint32_t aluExecute(ALUOp op, uint32_t lhs, uint32_t rhs);
// Execute supported R-type instruction using register indices.
void executeRType(class RegisterFile& regs, ALUOp op,
                  unsigned rd, unsigned rs, unsigned rt);
