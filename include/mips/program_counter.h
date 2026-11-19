// Assigned to: Zunain Nazir.
// Stores the current instruction byte address and handles sequential advancement.
#pragma once

#include <cstdint>

namespace mips {

class ProgramCounter {
public:
    void reset();
    std::uint32_t value() const;
    void set(std::uint32_t byteAddress);
    void advance();

private:
    std::uint32_t byteAddress_{};
};

} // namespace mips
