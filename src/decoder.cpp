#include "decoder.hpp"
#include "alu.hpp"
#include <stdexcept>

DecodedRType decodeRType(uint32_t word) {
    DecodedRType d{word, (word >> 26) & 0x3fu, (word >> 21) & 0x1fu,
                   (word >> 16) & 0x1fu, (word >> 11) & 0x1fu,
                   (word >> 6) & 0x1fu, word & 0x3fu};
    if (d.opcode != 0) throw std::invalid_argument("Not a MIPS R-type instruction (opcode != 0)");
    const ALUOp op = aluOpFromFunct(d.funct); // validates supported funct
    const bool isShift = op == ALUOp::SLL || op == ALUOp::SRL || op == ALUOp::SRA;
    if (isShift && d.rs != 0)
        throw std::invalid_argument("Fixed shifts require rs=0");
    if (!isShift && d.shamt != 0)
        throw std::invalid_argument("Non-shift R-type instructions require shamt=0");
    return d;
}

uint32_t encodeRType(unsigned rs, unsigned rt, unsigned rd,
                     unsigned shamt, unsigned funct) {
    if (rs > 31 || rt > 31 || rd > 31 || shamt > 31 || funct > 63)
        throw std::out_of_range("R-type encoding field out of range");
    return (rs << 21) | (rt << 16) | (rd << 11) | (shamt << 6) | funct;
}
