#include "alu.hpp"
#include "registers.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
int main() {
    RegisterFile r;
    r.write(0, 100); assert(r.read(0) == 0);
    r.write(9, 10); r.write(10, 20);
    executeRType(r, ALUOp::ADD, 8, 9, 10); assert(r.read(8) == 30);
    executeRType(r, ALUOp::SUB, 8, 9, 10); assert(r.read(8) == static_cast<uint32_t>(-10));
    r.write(9, 0x0f0fu); r.write(10, 0x00ffu);
    executeRType(r, ALUOp::AND, 8, 9, 10); assert(r.read(8) == 0x000fu);
    executeRType(r, ALUOp::OR, 8, 9, 10); assert(r.read(8) == 0x0fffu);
    r.write(9, static_cast<uint32_t>(-1)); r.write(10, 1);
    executeRType(r, ALUOp::SLT, 8, 9, 10); assert(r.read(8) == 1);
    executeRType(r, ALUOp::SLT, 8, 10, 9); assert(r.read(8) == 0);
    r.write(8, UINT32_MAX);
    executeRType(r, ALUOp::AND, 8, 8, 10); assert(r.read(8) == 1);
    r.write(9, static_cast<uint32_t>(INT32_MAX)); r.write(10, 1);
    bool overflowed = false;
    try { executeRType(r, ALUOp::ADD, 8, 9, 10); }
    catch (const std::overflow_error&) { overflowed = true; }
    assert(overflowed && r.read(8) == 1);
    bool badreg = false;
    try { r.read(32); } catch (const std::out_of_range&) { badreg = true; }
    assert(badreg);
    r.reset(); assert(r.read(8) == 0 && r.read(9) == 0);
    std::cout << "All register and ALU tests passed.\n";
}
