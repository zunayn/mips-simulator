// Assigned to: Zunain Nazir.
#include "mips/control.h"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
enum Flag {
    RegWrite = 1, MemRead = 2, MemWrite = 4, MemToReg = 8, Immediate = 16,
    DestinationRd = 32, Branch = 64, Jump = 128, Link = 256
};

unsigned flags(const mips::ControlSignals& s) {
    return (s.registerWrite ? RegWrite : 0) | (s.memoryRead ? MemRead : 0)
         | (s.memoryWrite ? MemWrite : 0) | (s.memoryToRegister ? MemToReg : 0)
         | (s.aluSourceImmediate ? Immediate : 0) | (s.destinationIsRd ? DestinationRd : 0)
         | (s.branch ? Branch : 0) | (s.jump ? Jump : 0) | (s.link ? Link : 0);
}
} // namespace

int main() {
    int checks = 0;
    const auto check = [&](bool condition, const std::string& name) {
        ++checks;
        if (!condition) throw std::runtime_error("Failed: " + name);
    };
    const auto rejects = [&](const std::function<void()>& action, const std::string& name) {
        bool rejected = false;
        try { action(); } catch (const std::exception&) { rejected = true; }
        check(rejected, name);
    };
    mips::ControlUnit control;
    RegisterFile registers;
    mips::RTypeInstruction r;
    mips::ITypeInstruction i;
    mips::JTypeInstruction j;
    check(r.format() == mips::InstructionFormat::RType
          && i.format() == mips::InstructionFormat::IType
          && j.format() == mips::InstructionFormat::JType, "instruction formats");

    struct RCase { unsigned function; ALUOp op; };
    const RCase rCases[] = {
        {0x20, ALUOp::ADD}, {0x21, ALUOp::ADDU}, {0x22, ALUOp::SUB},
        {0x23, ALUOp::SUBU}, {0x24, ALUOp::AND}, {0x25, ALUOp::OR},
        {0x26, ALUOp::XOR}, {0x27, ALUOp::NOR}, {0x2a, ALUOp::SLT},
        {0x2b, ALUOp::SLTU}, {0x00, ALUOp::SLL}, {0x02, ALUOp::SRL}, {0x03, ALUOp::SRA}
    };
    for (const auto& c : rCases) {
        r.functionCode = static_cast<std::uint8_t>(c.function);
        const auto s = control.decode(r);
        check(s.aluOperation == c.op && flags(s) == (RegWrite | DestinationRd),
              "R-type ALU selection and writeback: " + std::to_string(c.function));
    }
    check(control.nextAddress(0x00400000u, r, registers) == 0x00400004u,
          "ordinary instruction advances four bytes");

    struct ICase { unsigned opcode; ALUOp op; unsigned flags; };
    const ICase iCases[] = {
        {0x08, ALUOp::ADD, RegWrite | Immediate},
        {0x0c, ALUOp::AND, RegWrite | Immediate},
        {0x0d, ALUOp::OR, RegWrite | Immediate},
        {0x23, ALUOp::ADDU, RegWrite | Immediate | MemRead | MemToReg},
        {0x2b, ALUOp::ADDU, Immediate | MemWrite},
        {0x04, ALUOp::SUBU, Branch}, {0x05, ALUOp::SUBU, Branch}
    };
    for (const auto& c : iCases) {
        i.opcode = static_cast<std::uint8_t>(c.opcode);
        const auto s = control.decode(i);
        check(s.aluOperation == c.op && flags(s) == c.flags,
              "I-type control routing: " + std::to_string(c.opcode));
    }
    i.opcode = 0x04; i.rs = 8; i.rt = 9; i.immediate = 2;
    registers.write(8, 42); registers.write(9, 42);
    check(control.nextAddress(0x00400000u, i, registers) == 0x0040000cu, "BEQ taken");
    registers.write(9, 43);
    check(control.nextAddress(0x00400000u, i, registers) == 0x00400004u, "BEQ not taken");
    i.opcode = 0x05;
    check(control.nextAddress(0x00400000u, i, registers) == 0x0040000cu, "BNE taken");
    registers.write(9, 42);
    check(control.nextAddress(0x00400000u, i, registers) == 0x00400004u, "BNE not taken");
    i.opcode = 0x04; i.immediate = 0xfffd;
    check(control.nextAddress(0x00400008u, i, registers) == 0x00400000u,
          "negative offset branches backward from PC plus four");
    i.immediate = 0x8000;
    check(control.nextAddress(0x00400000u, i, registers) == 0x003e0004u, "minimum signed offset");
    i.immediate = 0x7fff;
    check(control.nextAddress(0x00400000u, i, registers) == 0x00420000u, "maximum signed offset");
    i.immediate = 0;
    check(control.nextAddress(0x00400000u, i, registers) == 0x00400004u, "zero offset");
    i.immediate = 0xfffe;
    check(control.nextAddress(0, i, registers) == 0xfffffffcu, "target wraps as 32-bit address");
    registers.write(8, 0x80000000u); registers.write(9, 0x7fffffffu);
    check(control.nextAddress(0x00400000u, i, registers) == 0x00400004u,
          "branch comparison cannot cause signed subtraction overflow");

    j.opcode = 0x02; j.target = 3;
    check(flags(control.decode(j)) == Jump, "J does not write a register");
    check(control.nextAddress(0x00400000u, j, registers) == 12, "J restores low two address bits");
    check(control.nextAddress(0x0ffffffcu, j, registers) == 0x1000000cu,
          "J takes upper address bits from PC plus four");
    j.opcode = 0x03;
    registers.write(31, 55);
    check(flags(control.decode(j)) == (Jump | Link | RegWrite), "JAL selects link writeback");
    check(control.nextAddress(0x00400000u, j, registers) == 12, "JAL jump address");
    check(registers.read(31) == 55 && registers.read(8) == 0x80000000u,
          "control calculates decisions without modifying registers");

    rejects([&] { control.nextAddress(2, j, registers); }, "unaligned current PC");
    rejects([&] { control.nextAddress(0xfffffffcu, j, registers); }, "checked sequential PC overflow");
    j.target = 0x04000000u;
    rejects([&] { control.decode(j); }, "oversized jump target");
    j.target = 0; j.opcode = 0x04;
    rejects([&] { control.decode(j); }, "branch opcode in J-type object");
    i.opcode = 0x02;
    rejects([&] { control.decode(i); }, "jump opcode in I-type object");
    i.opcode = 0x04; i.rs = 32;
    rejects([&] { control.decode(i); }, "invalid register field");
    r.opcode = 0x08;
    rejects([&] { control.decode(r); }, "nonzero R-type opcode");
    r.opcode = 0; r.functionCode = 0x3f;
    rejects([&] { control.decode(r); }, "unknown function code");
    r.functionCode = 0x20; r.shiftAmount = 1;
    rejects([&] { control.decode(r); }, "non-shift with shift amount");
    r.functionCode = 0; r.shiftAmount = 0; r.rs = 1;
    rejects([&] { control.decode(r); }, "fixed shift with nonzero rs");
    r.rs = 0; r.shiftAmount = 32;
    rejects([&] { control.decode(r); }, "oversized shift amount");
    struct WrongType : mips::Instruction {
        mips::InstructionFormat format() const override { return mips::InstructionFormat::RType; }
    } wrong;
    rejects([&] { control.decode(wrong); }, "format does not match concrete instruction type");
    std::cout << "All " << checks << " control-unit checks passed.\n";
}
