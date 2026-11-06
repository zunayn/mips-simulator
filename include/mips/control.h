// Assigned to: Zunain Nazir.
// Blueprint: processor control signals and branch/jump address selection.
#pragma once

#include "mips/alu.h"
#include "mips/instruction.h"
#include "mips/registers.h"

#include <cstdint>

namespace mips {

struct ControlSignals {
    ALUOperation aluOperation{};
    bool registerWrite{};
    bool memoryRead{};
    bool memoryWrite{};
    bool memoryToRegister{};
    bool aluSourceImmediate{};
    bool destinationIsRd{};
    bool branch{};
    bool jump{};
    bool link{};
};

class ControlUnit {
public:
    ControlSignals decode(const Instruction& instruction) const;
    std::uint32_t nextAddress(std::uint32_t currentAddress,
                              const Instruction& instruction,
                              const RegisterFile& registers) const;
};

} // namespace mips
