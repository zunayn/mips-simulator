#pragma once
#include <array>
#include <cstdint>
#include <iosfwd>

// MIPS32 general-purpose register bank. Register 0 is hardwired to zero.
class RegisterFile {
public:
    RegisterFile();
    uint32_t read(unsigned index) const;
    void write(unsigned index, uint32_t value);
    void reset();
    void dump(std::ostream& out) const;
private:
    std::array<uint32_t, 32> values_{};
};
