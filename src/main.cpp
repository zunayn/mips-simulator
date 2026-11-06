#include "alu.hpp"
#include "registers.hpp"
#include <iostream>

// Core-component demo, NOT yet a full MIPS assembly parser or CPU simulator.
int main() {
    RegisterFile regs;
    regs.write(9, 10);  // $t1 = 10
    regs.write(10, 20); // $t2 = 20
    executeRType(regs, ALUOp::ADD, 8, 9, 10); // add $t0, $t1, $t2
    std::cout << "Executed: add $t0, $t1, $t2\nExpected $t0 = 30\nActual $t0 = "
              << regs.read(8) << "\n";
    return regs.read(8) == 30 ? 0 : 1;
}
