// Assigned to: Haddon Stauffer.
#include "mips/memory.h"

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <stdexcept>

namespace mips {
namespace {

// Both loads and stores use the same alignment and bounds checks.
void validateWordAccess(std::uint32_t address, std::size_t sizeBytes) {
    if (address % 4 != 0) {
        throw std::invalid_argument("Word address must be divisible by 4");
    }
    if (sizeBytes < 4 || static_cast<std::uint64_t>(address) > sizeBytes - 4) {
        throw std::out_of_range("Word address is outside memory");
    }
}

}


// Memory::initialize - allocates the mem and fills w 0s
void Memory::initialize(std::size_t sizeBytes) {
    constexpr std::uint64_t maxBytes = std::uint64_t{1} << 32;
    if (sizeBytes == 0 || sizeBytes % 4 != 0 || sizeBytes > maxBytes) {
        throw std::invalid_argument("Memory size must be a positive multiple of 4, at most 2^32 bytes");
    }
    bytes_.assign(sizeBytes, 0);
}

// Memory::reset - resets the mem and fills it w 0s
void Memory::reset() {
    std::fill(bytes_.begin(), bytes_.end(), 0);
}

// Memory::loadWord - reads 4 bytes from mem and combines into word (32 bits)
std::uint32_t Memory::loadWord(std::uint32_t byteAddress) const {
    validateWordAccess(byteAddress, bytes_.size());
    std::uint32_t result = 0;
    for (unsigned i = 0; i < 4; ++i) {
        result |= static_cast<std::uint32_t>(bytes_[static_cast<std::size_t>(byteAddress) + i]) << (8 * i);
    }
    return result;
}

// Memory::storeWord - takes a word and splits it into 4 bytes to store it in mem
void Memory::storeWord(std::uint32_t byteAddress, std::uint32_t value) {
    validateWordAccess(byteAddress, bytes_.size());
    for (unsigned i = 0; i < 4; ++i) {
        bytes_[static_cast<std::size_t>(byteAddress) + i] =
            static_cast<std::uint8_t>((value >> (8 * i)) & 0xFFu);
    }
}

//Memory::display - prints nonzero words and their addresses for debugging
void Memory::display(std::ostream& output) const {
    const auto originalFlags = output.flags();
    const auto originalFill = output.fill();
    bool foundNonzero = false;

    for (std::size_t address = 0; address + 3 < bytes_.size(); address += 4) {
        std::uint32_t value = 0;
        for (unsigned i = 0; i < 4; ++i) {
            value |= static_cast<std::uint32_t>(bytes_[address + i]) << (8 * i);
        }
        if (value != 0) {
            foundNonzero = true;
            output << "0x" << std::hex << std::setfill('0') << std::setw(8)
                   << address << ": 0x" << std::setw(8) << value << '\n';
        }
    }

    if (!foundNonzero) {
        output << "Memory contains no nonzero words.\n";
    }
    output.flags(originalFlags);
    output.fill(originalFill);
}

} // namespace mips
