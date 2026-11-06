#include "registers.hpp"
#include <iomanip>
#include <ostream>
#include <stdexcept>

RegisterFile::RegisterFile() { reset(); }
void RegisterFile::reset() { values_.fill(0); }
uint32_t RegisterFile::read(unsigned index) const {
    if (index >= values_.size()) throw std::out_of_range("Register index must be 0..31");
    return values_[index];
}
void RegisterFile::write(unsigned index, uint32_t value) {
    if (index >= values_.size()) throw std::out_of_range("Register index must be 0..31");
    if (index != 0) values_[index] = value;
}
void RegisterFile::dump(std::ostream& out) const {
    for (unsigned i = 0; i < values_.size(); ++i) {
        out << "r" << std::setw(2) << std::setfill('0') << i
            << " = 0x" << std::hex << std::setw(8) << values_[i] << std::dec << '\n';
    }
}
