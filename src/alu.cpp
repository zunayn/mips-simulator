#include "alu.hpp"
#include "registers.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>

uint32_t aluExecute(ALUOp op, uint32_t lhs, uint32_t rhs) {
    switch (op) {
        case ALUOp::ADD: {
            const int64_t result = static_cast<int64_t>(static_cast<int32_t>(lhs))
                                 + static_cast<int64_t>(static_cast<int32_t>(rhs));
            if (result > INT32_MAX || result < INT32_MIN)
                throw std::overflow_error("MIPS signed ADD overflow");
            return static_cast<uint32_t>(static_cast<int32_t>(result));
        }
        case ALUOp::SUB: {
            const int64_t result = static_cast<int64_t>(static_cast<int32_t>(lhs))
                                 - static_cast<int64_t>(static_cast<int32_t>(rhs));
            if (result > INT32_MAX || result < INT32_MIN)
                throw std::overflow_error("MIPS signed SUB overflow");
            return static_cast<uint32_t>(static_cast<int32_t>(result));
        }
        case ALUOp::AND: return lhs & rhs;
        case ALUOp::OR:  return lhs | rhs;
        case ALUOp::SLT: return static_cast<int32_t>(lhs) < static_cast<int32_t>(rhs) ? 1u : 0u;
    }
    throw std::invalid_argument("Unsupported ALU operation");
}

void executeRType(RegisterFile& regs, ALUOp op, unsigned rd, unsigned rs, unsigned rt) {
    // Read all operands before writeback, so rd is allowed to equal rs or rt.
    const auto lhs = regs.read(rs);
    const auto rhs = regs.read(rt);
    // Overflow or invalid index prevents any writeback.
    if (rd >= 32) throw std::out_of_range("Destination register must be 0..31");
    regs.write(rd, aluExecute(op, lhs, rhs));
}
