#pragma once
#include <cstdint>

// Decoder boundary: Timothy can replace this function while keeping the
// DecodedRType data contract and the downstream executor unchanged.
struct DecodedRType {
    uint32_t raw;
    unsigned opcode, rs, rt, rd, shamt, funct;
};

// This milestone intentionally accepts only the supported R-type subset.
// Invalid/unsupported encodings throw std::invalid_argument.
DecodedRType decodeRType(uint32_t instruction);

// Helper for tests and demonstration; not a general-purpose assembler.
uint32_t encodeRType(unsigned rs, unsigned rt, unsigned rd,
                     unsigned shamt, unsigned funct);
