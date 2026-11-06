#include "alu.hpp"
#include "registers.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace {
// This conversion preserves MIPS32 two's complement interpretation without
// relying on implementation-defined unsigned-to-signed narrowing.
int64_t asSigned32(uint32_t value) {
    return value <= 0x7fffffffu ? static_cast<int64_t>(value)
                                : static_cast<int64_t>(value) - (int64_t{1} << 32);
}

uint32_t checkedSigned(int64_t result) {
    if (result > INT32_MAX || result < INT32_MIN)
        throw std::overflow_error("MIPS signed arithmetic overflow");
    return static_cast<uint32_t>(result);
}
}

ALUResult aluEvaluate(ALUOp op, uint32_t lhs, uint32_t rhs, unsigned shamt) {
    if (shamt > 31) throw std::out_of_range("MIPS shift amount must be 0..31");
    uint32_t result = 0;
    switch (op) {
        case ALUOp::ADD:  result = checkedSigned(asSigned32(lhs) + asSigned32(rhs)); break;
        case ALUOp::ADDU: result = lhs + rhs; break; // 32-bit modular addition
        case ALUOp::SUB:  result = checkedSigned(asSigned32(lhs) - asSigned32(rhs)); break;
        case ALUOp::SUBU: result = lhs - rhs; break; // 32-bit modular subtraction
        case ALUOp::AND:  result = lhs & rhs; break;
        case ALUOp::OR:   result = lhs | rhs; break;
        case ALUOp::XOR:  result = lhs ^ rhs; break;
        case ALUOp::NOR:  result = ~(lhs | rhs); break;
        case ALUOp::SLT:  result = asSigned32(lhs) < asSigned32(rhs) ? 1u : 0u; break;
        case ALUOp::SLTU: result = lhs < rhs ? 1u : 0u; break;
        case ALUOp::SLL:  result = rhs << shamt; break;
        case ALUOp::SRL:  result = rhs >> shamt; break;
        case ALUOp::SRA: {
            // Arithmetic right shift with explicit sign extension (portable C++17).
            result = rhs >> shamt;
            if ((rhs & 0x80000000u) && shamt != 0)
                result |= (~uint32_t{0}) << (32 - shamt);
            break;
        }
        default: throw std::invalid_argument("Unsupported ALU operation");
    }
    return {result, result == 0};
}

uint32_t aluExecute(ALUOp op, uint32_t lhs, uint32_t rhs) {
    return aluEvaluate(op, lhs, rhs).value;
}

ALUOp aluOpFromFunct(unsigned funct) {
    switch (funct) {
        case 0x20: return ALUOp::ADD;
        case 0x21: return ALUOp::ADDU;
        case 0x22: return ALUOp::SUB;
        case 0x23: return ALUOp::SUBU;
        case 0x24: return ALUOp::AND;
        case 0x25: return ALUOp::OR;
        case 0x26: return ALUOp::XOR;
        case 0x27: return ALUOp::NOR;
        case 0x2a: return ALUOp::SLT;
        case 0x2b: return ALUOp::SLTU;
        case 0x00: return ALUOp::SLL;
        case 0x02: return ALUOp::SRL;
        case 0x03: return ALUOp::SRA;
        default: throw std::invalid_argument("Unsupported MIPS R-type funct field");
    }
}

void executeRType(RegisterFile& regs, ALUOp op,
                  unsigned rd, unsigned rs, unsigned rt, unsigned shamt) {
    if (rd >= 32) throw std::out_of_range("Destination register must be 0..31");
    // rd may alias rs or rt; snapshot both operands before writing.
    const uint32_t lhs = regs.read(rs);
    const uint32_t rhs = regs.read(rt);
    const ALUResult result = aluEvaluate(op, lhs, rhs, shamt);
    regs.write(rd, result.value);
}
