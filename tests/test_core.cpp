#include "alu.hpp"
#include "registers.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>

int main() {
    unsigned checks = 0;
    auto check = [&](bool passed) { ++checks; if (!passed) throw std::runtime_error("Test failed: " + std::to_string(checks)); };
    RegisterFile regs;
    regs.write(0, 100); check(regs.read(0) == 0);
    regs.write(9, 10); regs.write(10, 20);
    executeRType(regs, ALUOp::ADD, 8, 9, 10); check(regs.read(8) == 30);
    executeRType(regs, ALUOp::SUB, 8, 9, 10); check(regs.read(8) == static_cast<uint32_t>(-10));
    check(aluExecute(ALUOp::ADDU, 0xffffffffu, 1) == 0);
    check(aluExecute(ALUOp::SUBU, 0, 1) == 0xffffffffu);
    check(aluExecute(ALUOp::AND, 0xf0f0u, 0x0ff0u) == 0x00f0u);
    check(aluExecute(ALUOp::OR, 0xf0f0u, 0x0ff0u) == 0xfff0u);
    check(aluExecute(ALUOp::XOR, 0xf0f0u, 0x0ff0u) == 0xff00u);
    check(aluExecute(ALUOp::NOR, 0xffffffffu, 0u) == 0u);
    check(aluExecute(ALUOp::SLT, 0xffffffffu, 1u) == 1);
    check(aluExecute(ALUOp::SLTU, 0xffffffffu, 1u) == 0);
    check(aluEvaluate(ALUOp::SLL, 0u, 1u, 31).value == 0x80000000u);
    check(aluEvaluate(ALUOp::SRL, 0u, 0x80000000u, 31).value == 1);
    check(aluEvaluate(ALUOp::SRA, 0u, 0x80000000u, 31).value == 0xffffffffu);
    check(aluEvaluate(ALUOp::SRA, 0u, 0x80000000u, 0).value == 0x80000000u);
    check(aluEvaluate(ALUOp::SRA, 0u, 0x7fffffffu, 1).value == 0x3fffffffu);
    check(aluEvaluate(ALUOp::XOR, 123, 123).zero);
    check(!aluEvaluate(ALUOp::ADD, 10, 20).zero);
    check(aluOpFromFunct(0x20) == ALUOp::ADD);
    check(aluOpFromFunct(0x03) == ALUOp::SRA);
    bool caught = false;
    try { aluOpFromFunct(0x3f); } catch (const std::invalid_argument&) { caught = true; }
    check(caught);
    caught = false;
    try { aluEvaluate(ALUOp::SLL, 0, 1, 32); } catch (const std::out_of_range&) { caught = true; }
    check(caught);
    caught = false;
    try { aluExecute(ALUOp::ADD, 0x7fffffffu, 1); } catch (const std::overflow_error&) { caught = true; }
    check(caught);
    caught = false;
    try { aluExecute(ALUOp::SUB, 0x80000000u, 1); } catch (const std::overflow_error&) { caught = true; }
    check(caught);
    regs.write(8, 55);
    regs.write(9, 0x7fffffffu); regs.write(10, 1);
    caught = false;
    try { executeRType(regs, ALUOp::ADD, 8, 9, 10); } catch (const std::overflow_error&) { caught = true; }
    check(caught && regs.read(8) == 55);
    regs.write(10, 0x80000000u);
    executeRType(regs, ALUOp::SRA, 8, 0, 10, 4);
    check(regs.read(8) == 0xf8000000u);
    regs.write(8, 2);
    executeRType(regs, ALUOp::SLL, 8, 0, 8, 2);
    check(regs.read(8) == 8);
    executeRType(regs, ALUOp::ADDU, 0, 9, 10); check(regs.read(0) == 0);
    caught = false;
    try { regs.read(32); } catch (const std::out_of_range&) { caught = true; }
    check(caught);
    regs.reset(); check(regs.read(8) == 0 && regs.read(9) == 0);
    std::cout << "All " << checks << " register and ALU checks passed.\n";
}
