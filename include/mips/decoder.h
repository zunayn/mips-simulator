// Assigned to: Timothy Sparks.
// Blueprint: decode instruction words into R-, I-, or J-type objects.
#pragma once

#include "mips/instruction.h"

#include <cstdint>
#include <memory>

namespace mips {

class InstructionDecoder {
public:
    std::unique_ptr<Instruction> decode(std::uint32_t word) const;
};

} // namespace mips
