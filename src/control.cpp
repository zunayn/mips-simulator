// Assigned to: Zunain Nazir.
#include "mips/control.h"
#include "mips/program_counter.h"

#include <stdexcept>

namespace mips {
namespace {

// The public interface accepts a base reference. Check the actual object before
// reading format-specific fields, including objects constructed directly in tests.
template <typename T>
const T& fields(const Instruction& instruction) {
    const auto* typed = dynamic_cast<const T*>(&instruction);
    if (!typed) throw std::invalid_argument("Instruction format and object type disagree");
    return *typed;
}

void validateRegister(unsigned index) {
    if (index > 31) throw std::out_of_range("Control register index must be 0..31");
}

} // namespace

ControlSignals ControlUnit::decode(const Instruction& instruction) const {
    ControlSignals signals{}; // Every action is disabled until selected below.

    switch (instruction.format()) {
        case InstructionFormat::RType: {
            const auto& r = fields<RTypeInstruction>(instruction);
            if (r.opcode != 0) throw std::invalid_argument("R-type opcode must be zero");
            validateRegister(r.rs);
            validateRegister(r.rt);
            validateRegister(r.rd);
            if (r.shiftAmount > 31) throw std::out_of_range("Shift amount must be 0..31");
            signals.aluOperation = aluOpFromFunct(r.functionCode);
            const bool shift = signals.aluOperation == ALUOp::SLL
                            || signals.aluOperation == ALUOp::SRL
                            || signals.aluOperation == ALUOp::SRA;
            if ((shift && r.rs != 0) || (!shift && r.shiftAmount != 0)) {
                throw std::invalid_argument("Invalid R-type shift fields");
            }
            signals.registerWrite = true;
            signals.destinationIsRd = true;
            return signals;
        }
        case InstructionFormat::IType: {
            const auto& i = fields<ITypeInstruction>(instruction);
            validateRegister(i.rs);
            validateRegister(i.rt);
            switch (i.opcode) {
                case 0x08: // addi
                    signals.aluOperation = ALUOp::ADD;
                    signals.registerWrite = true;
                    signals.aluSourceImmediate = true;
                    break;
                case 0x0c: // andi
                case 0x0d: // ori
                    signals.aluOperation = i.opcode == 0x0c ? ALUOp::AND : ALUOp::OR;
                    signals.registerWrite = true;
                    signals.aluSourceImmediate = true;
                    break;
                case 0x23: // lw
                case 0x2b: // sw
                    // Effective addresses use wrapping addition, not signed ADD traps.
                    signals.aluOperation = ALUOp::ADDU;
                    signals.aluSourceImmediate = true;
                    signals.memoryRead = i.opcode == 0x23;
                    signals.memoryWrite = i.opcode == 0x2b;
                    signals.memoryToRegister = signals.memoryRead;
                    signals.registerWrite = signals.memoryRead;
                    break;
                case 0x04: // beq
                case 0x05: // bne
                    signals.aluOperation = ALUOp::SUBU;
                    signals.branch = true;
                    break;
                default:
                    throw std::invalid_argument("Unsupported I-type control opcode");
            }
            return signals;
        }
        case InstructionFormat::JType: {
            const auto& j = fields<JTypeInstruction>(instruction);
            if (j.opcode != 0x02 && j.opcode != 0x03) {
                throw std::invalid_argument("Unsupported J-type control opcode");
            }
            if (j.target > 0x03ffffffu) {
                throw std::out_of_range("Jump target must fit 26 bits");
            }
            signals.jump = true;
            signals.link = j.opcode == 0x03;
            signals.registerWrite = signals.link;
            return signals;
        }
    }
    throw std::invalid_argument("Unknown instruction format");
}

std::uint32_t ControlUnit::nextAddress(std::uint32_t currentAddress,
                                     const Instruction& instruction,
                                     const RegisterFile& registers) const {
    const auto signals = decode(instruction); // Validate before choosing a path.
    ProgramCounter sequential;
    sequential.set(currentAddress);
    sequential.advance(); // Preserve this branch's checked PC + 4 behavior.

    if (signals.branch) {
        const auto& i = fields<ITypeInstruction>(instruction);
        const bool equal = registers.read(i.rs) == registers.read(i.rt);
        const bool taken = i.opcode == 0x04 ? equal : !equal;
        if (taken) {
            // Convert the raw 16-bit field to a signed offset without narrowing
            // an out-of-range unsigned value or left-shifting a negative integer.
            const std::int64_t offset = i.immediate < 0x8000u
                ? i.immediate : static_cast<std::int64_t>(i.immediate) - 0x10000;
            return static_cast<std::uint32_t>(sequential.value() + offset * 4);
        }
    }
    if (signals.jump) {
        const auto& j = fields<JTypeInstruction>(instruction);
        return (sequential.value() & 0xf0000000u) | (j.target << 2);
    }
    return sequential.value();
}

} // namespace mips
