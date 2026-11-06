// Assigned to: Tony McFarlan.
// Blueprint: 32 registers, register access, and the zero-register rule.
#pragma once

#include <array>
#include <cstdint>

namespace mips {

class RegisterFile {
public:
    void reset();
    std::uint32_t read(std::uint8_t index) const;
    void write(std::uint8_t index, std::uint32_t value);

private:
    std::array<std::uint32_t, 32> registers_{};
};

} // namespace mips
