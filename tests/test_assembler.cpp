#include "mips/assembler.h"
#include "mips/decoder.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    unsigned checks = 0;
    auto check = [&](bool passed, const std::string& label) {
        ++checks;
        if (!passed) throw std::runtime_error("FAILED: " + label);
    };
    auto rejects = [&](const std::string& text, const std::string& label) {
        std::istringstream source(text);
        bool failedWithLineNumber = false;
        try {
            mips::assemble(source);
        } catch (const std::invalid_argument& error) {
            failedWithLineNumber = std::string(error.what()).find("Assembly line 1:")
                                 != std::string::npos;
        }
        check(failedWithLineNumber, label);
    };

    std::istringstream source(
        "# comment\n"
        "add $t0, $t1, $t2 # aliases\n"
        "SLL $13, $11, 2\n"
        "subu $8, $9, $10\n"
        "nop\n");
    const auto words = mips::assemble(source);
    check(words == std::vector<std::uint32_t>{
        encodeRType(9, 10, 8, 0, 0x20),
        encodeRType(0, 11, 13, 2, 0x00),
        encodeRType(9, 10, 8, 0, 0x23),
        0u
    }, "assemble R-type instructions, aliases, shifts, comments, and nop");

    rejects("addi $t0, $t1, 1\n", "reject unsupported instruction");
    rejects("add $t0, $t1\n", "reject wrong operand count");
    rejects("add $t0 $t1, $t2\n", "require comma-separated operands");
    rejects("add $t0, $t1, $32\n", "reject invalid register");
    rejects("sll $t0, $t1, 32\n", "reject out-of-range shift amount");
    rejects("nop $zero\n", "reject operands to nop");

    std::cout << "All " << checks << " assembler checks passed.\n";
}
