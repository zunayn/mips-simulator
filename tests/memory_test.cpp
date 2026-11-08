#include "mips/memory.h"

#include <cassert>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

int main() {
    mips::Memory memory;
    bool threw = false;
    try { memory.loadWord(0); } catch (const std::out_of_range&) { threw = true; }
    assert(threw); // Cannot read memory before initialization.

    for (std::size_t badSize : {std::size_t{0}, std::size_t{3}, std::size_t{5}}) {
        threw = false;
        try { memory.initialize(badSize); } catch (const std::invalid_argument&) { threw = true; }
        assert(threw);
    }

    memory.initialize(16);
    assert(memory.loadWord(0) == 0);
    memory.storeWord(4, 0x12345678u);
    assert(memory.loadWord(4) == 0x12345678u);
    memory.storeWord(12, 0xDEADBEEFu); // Last aligned word in 16 bytes.
    assert(memory.loadWord(12) == 0xDEADBEEFu);
    assert(memory.loadWord(0) == 0);

    threw = false;
    try { memory.storeWord(6, 0xFFFFFFFFu); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    assert(memory.loadWord(4) == 0x12345678u); // Failed write changed nothing.

    threw = false;
    try { memory.loadWord(16); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);

    std::ostringstream report;
    report << std::dec << std::setfill(' ');
    memory.display(report);
    assert(report.str().find("0x00000004: 0x12345678") != std::string::npos);
    assert(report.str().find("0x0000000c: 0xdeadbeef") != std::string::npos);
    assert((report.flags() & std::ios_base::basefield) == std::ios_base::dec);
    assert(report.fill() == ' ');

    memory.reset();
    assert(memory.loadWord(4) == 0);
    assert(memory.loadWord(12) == 0);
    std::ostringstream empty;
    memory.display(empty);
    assert(empty.str().find("no nonzero words") != std::string::npos);
    memory.storeWord(12, 2); // Reset preserved memory allocation.
    assert(memory.loadWord(12) == 2);
}
