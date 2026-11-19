// Assigned to: Zunain Nazir.
#include "mips/program_counter.h"

#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    int checks = 0;
    const auto check = [&](bool condition, const std::string& name) {
        ++checks;
        if (!condition) throw std::runtime_error("Failed: " + name);
    };

    mips::ProgramCounter pc;
    check(pc.value() == 0, "initial address");
    pc.advance();
    check(pc.value() == 4, "advance by one instruction");
    pc.set(0x00400000u);
    check(pc.value() == 0x00400000u, "set text address");
    pc.advance();
    check(pc.value() == 0x00400004u, "advance from text address");

    for (const auto address : {1u, 2u, 3u, 0x00400002u}) {
        bool rejected = false;
        try {
            pc.set(address);
        } catch (const std::invalid_argument&) {
            rejected = true;
        }
        check(rejected && pc.value() == 0x00400004u,
              "reject unaligned address without changing PC");
    }

    pc.set(0xfffffff8u);
    pc.advance();
    check(pc.value() == 0xfffffffcu, "last valid advancement");
    bool overflow = false;
    try {
        pc.advance();
    } catch (const std::overflow_error&) {
        overflow = true;
    }
    check(overflow && pc.value() == 0xfffffffcu,
          "overflow preserves current address");

    pc.reset();
    check(pc.value() == 0, "reset clears address");
    pc.advance();
    check(pc.value() == 4, "advance after reset");
    std::cout << "All " << checks << " program-counter checks passed.\n";
}
