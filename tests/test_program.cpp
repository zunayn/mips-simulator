#include "cpu.hpp"
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    int n = 0;
    auto ok = [&](bool value, const std::string& label) {
        ++n;
        if (!value) throw std::runtime_error("FAILED: " + label);
    };
    auto rejects = [&](const std::function<void()>& fn, const std::string& label) {
        bool failed = false;
        try { fn(); } catch (const std::exception&) { failed = true; }
        ok(failed, label);
    };
    const std::vector<uint32_t> program = {
        encodeRType(9,10,8,0,0x20), encodeRType(8,9,11,0,0x22),
        encodeRType(8,11,12,0,0x24), encodeRType(0,11,13,2,0), 0u
    };
    SingleCycleCPU cpu;
    cpu.registers.write(9,10); cpu.registers.write(10,20);
    cpu.loadProgram(program);
    ok(cpu.instructions.size()==5, "instruction count");
    ok(cpu.instructions.fetch(0x00400000u)==program[0], "first fetch");
    ok(cpu.instructions.fetch(0x00400010u)==0, "last fetch");
    rejects([&]{ cpu.instructions.fetch(0x00400002u); }, "misaligned fetch");
    rejects([&]{ cpu.instructions.fetch(0x00400014u); }, "past-end fetch");
    rejects([&]{ cpu.instructions.fetch(0x003ffffcu); }, "before-base fetch");
    ok(!cpu.halted(), "not halted before run");
    auto first=cpu.step();
    ok(first.pcBefore==0x00400000u && first.pcAfter==0x00400004u, "PC increments");
    ok(cpu.registers.read(8)==30, "first ALU write-back");
    const auto remaining=cpu.run();
    ok(remaining.size()==4 && cpu.halted(), "run remaining and halt");
    ok(cpu.pc==0x00400014u, "final PC");
    ok(cpu.registers.read(11)==20, "dependent SUB");
    ok(cpu.registers.read(12)==20, "dependent AND");
    ok(cpu.registers.read(13)==80, "dependent SLL");
    ok(remaining.back().assembly=="nop", "nop trace");
    ok(cpu.registers.read(0)==0, "zero preserved");
    rejects([&]{ cpu.step(); }, "cannot fetch after program end");
    cpu.loadProgram({0xffffffffu});
    auto pc=cpu.pc; auto t0=cpu.registers.read(8);
    rejects([&]{ cpu.step(); }, "invalid encoding");
    ok(cpu.pc==pc && cpu.registers.read(8)==t0, "invalid instruction leaves state intact");
    cpu.registers.write(9,0x7fffffffu); cpu.registers.write(10,1);
    cpu.loadProgram({program[0]}); pc=cpu.pc; t0=cpu.registers.read(8);
    rejects([&]{ cpu.step(); }, "signed overflow");
    ok(cpu.pc==pc && cpu.registers.read(8)==t0, "overflow leaves state intact");
    rejects([&]{ cpu.loadProgram(program,0x00400002u); }, "base alignment");
    rejects([&]{ cpu.loadProgram(program,0xfffffff0u); }, "base space overflow");
    const char* temp="mips_temp_program_test.hex";
    { std::ofstream out(temp); out << "# comment\n\n" << "0x012A4020 # add\n" << "00000000\n"; }
    cpu.loadProgramFile(temp);
    ok(cpu.instructions.size()==2 && cpu.instructions.fetch(cpu.pc)==program[0], "hex file load");
    { std::ofstream out(temp); out << "xyz\n"; }
    rejects([&]{ cpu.loadProgramFile(temp); }, "invalid hex file");
    ok(cpu.instructions.size()==2, "invalid file does not replace program");
    { std::ofstream out(temp); out << "0x012A4020 0x00000000\n"; }
    rejects([&]{ cpu.loadProgramFile(temp); }, "two words per line");
    std::remove(temp);
    cpu.loadProgram({});
    ok(cpu.halted() && cpu.run().empty(), "empty program");
    cpu.registers.write(9,10); cpu.registers.write(10,20);
    cpu.loadProgram(program);
    rejects([&]{ cpu.run(2); }, "instruction limit");
    ok(cpu.pc==0x00400008u, "instruction limit stops after exactly two");
    std::cout << "All " << n << " instruction-memory/program checks passed.\n";
}
