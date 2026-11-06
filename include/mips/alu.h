// Assigned to: Tony McFarlan.
// Blueprint: arithmetic/logical operations used by the datapath.
#pragma once

#include <cstdint>

namespace mips {

enum class ALUOperation { Add, Subtract, And, Or, SetLessThan };

class ALU {
public:
    std::uint32_t execute(ALUOperation operation,
                          std::uint32_t left,
                          std::uint32_t right) const;
};

} // namespace mips
