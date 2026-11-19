// Assigned to: Zunain Nazir.
#include "mips/program_counter.h"

#include <limits>
#include <stdexcept>

namespace mips {

void ProgramCounter::reset() {
    byteAddress_ = 0;
}

std::uint32_t ProgramCounter::value() const {
    return byteAddress_;
}

void ProgramCounter::set(std::uint32_t byteAddress) {
    if (byteAddress % 4 != 0) {
        throw std::invalid_argument("PC must be aligned to a four-byte instruction");
    }
    byteAddress_ = byteAddress;
}

void ProgramCounter::advance() {
    if (byteAddress_ > std::numeric_limits<std::uint32_t>::max() - 4) {
        throw std::overflow_error("PC + 4 overflow");
    }
    byteAddress_ += 4;
}

} // namespace mips
