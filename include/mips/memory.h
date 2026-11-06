// Assigned to: Haddon Stauffer.
// Blueprint: data memory, initialization, word access, and memory display.
#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <vector>

namespace mips {

class Memory {
public:
    void initialize(std::size_t sizeBytes);
    void reset();
    std::uint32_t loadWord(std::uint32_t byteAddress) const;
    void storeWord(std::uint32_t byteAddress, std::uint32_t value);
    void display(std::ostream& output) const;

private:
    std::vector<std::uint8_t> bytes_;
};

} // namespace mips
