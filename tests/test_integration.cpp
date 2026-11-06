#include "cpu.hpp"
#include <cstdint>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    int checks = 0;
    auto check = [&](bool condition, const std::string& name) {
        ++checks;
        if (!condition) throw std::runtime_error("Failed: " + name);
    };
    auto reject = [&](const std::function<void()>& fn, const std::string& name) {
        bool rejected = false;
        try { fn(); } catch (const std::exception&) { rejected = true; }
        check(rejected, name);
    };
    SingleCycleCPU cpu;
    cpu.registers.write(9, 10);
    cpu.registers.write(10, 20);
    const auto t = cpu.stepRType(0x012A4020u);
    check(t.decoded.rs == 9 && t.decoded.rt == 10 && t.decoded.rd == 8, "decode registers");
    check(t.operation == ALUOp::ADD && t.result == 30, "ALU ADD");
    check(cpu.registers.read(8) == 30, "register write-back");
    check(cpu.pc == 0x00400004u && t.pcBefore == 0x00400000u, "PC + 4");
    check(t.assembly == "add $8, $9, $10", "disassembly");
    check(formatTrace(t).find("DECODE") != std::string::npos, "trace");
    struct Case { unsigned funct; uint32_t a,b,expected; } cases[] = {
      {0x21,0xffffffffu,1,0},{0x22,30,20,10},{0x23,0,1,0xffffffffu},
      {0x24,0xf0,0x0f,0},{0x25,0xf0,0x0f,0xff},
      {0x26,0xf0,0x0f,0xff},{0x27,0xf0,0x0f,0xffffff00u},
      {0x2a,0xffffffffu,1,1},{0x2b,0xffffffffu,1,0}
    };
    for (const auto& tc : cases) {
        cpu.registers.write(9,tc.a); cpu.registers.write(10,tc.b);
        const auto got=cpu.stepRType(encodeRType(9,10,8,0,tc.funct));
        check(got.result==tc.expected && cpu.registers.read(8)==tc.expected,
              "R-type funct " + std::to_string(tc.funct));
    }
    for (const auto& sc : {std::pair<unsigned,uint32_t>{0x00,0x1u},
                           {0x02,0x80000000u},{0x03,0x80000000u}}) {
        cpu.registers.write(10,sc.second);
        const auto got=cpu.stepRType(encodeRType(0,10,8,4,sc.first));
        const uint32_t expected=sc.first==0x00?0x10u:(sc.first==0x02?0x08000000u:0xf8000000u);
        check(got.result==expected, "shift instruction");
    }
    const uint32_t beforeZero = cpu.registers.read(0);
    const auto nop=cpu.stepRType(0);
    check(!nop.regWrite && cpu.registers.read(0)==beforeZero && nop.assembly=="nop", "nop");
    reject([&]{decodeRType(0x21280001u);}, "reject I-type");
    reject([&]{decodeRType(encodeRType(9,10,8,1,0x20));}, "non-shift shamt");
    reject([&]{decodeRType(encodeRType(9,10,8,4,0x00));}, "shift rs");
    reject([&]{decodeRType(encodeRType(9,10,8,0,0x3f));}, "unknown funct");
    reject([&]{encodeRType(32,0,0,0,0x20);}, "out-of-range encoder");
    cpu.registers.write(8,77); cpu.registers.write(9,0x7fffffffu); cpu.registers.write(10,1);
    const uint32_t pcBefore = cpu.pc;
    reject([&]{cpu.stepRType(0x012A4020u);}, "overflow trap");
    check(cpu.pc==pcBefore && cpu.registers.read(8)==77, "overflow atomicity");
    reject([&]{cpu.stepRType(0xffffffffu);}, "invalid instruction");
    check(cpu.pc==pcBefore && cpu.registers.read(8)==77, "invalid instruction atomicity");
    cpu.registers.write(9,0);
    const auto zero=cpu.stepRType(encodeRType(9,9,8,0,0x26));
    check(zero.zero && zero.result==0, "zero flag");
    std::cout << "All " << checks << " 32-bit decoding and single-cycle integration checks passed.\n";
}
