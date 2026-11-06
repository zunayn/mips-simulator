// Assigned to: Timothy Sparks.
// Blueprint: assembly input, labels, operands, and instruction encoding.
#pragma once

#include <cstdint>
#include <istream>
#include <vector>

namespace mips {

class InstructionParser {
public:
    std::vector<std::uint32_t> parse(std::istream& assembly) const;
};

} // namespace mips
