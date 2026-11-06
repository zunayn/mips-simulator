// Assigned to: Timothy Sparks.
// Blueprint: instruction formats and decoded fields; no execution behavior.
#pragma once

#include <cstdint>

namespace mips {

enum class InstructionFormat { RType, IType, JType };

class Instruction {
public:
    virtual ~Instruction() = default;
    virtual InstructionFormat format() const = 0;

    std::uint8_t opcode{};
};

class RTypeInstruction : public Instruction {
public:
    InstructionFormat format() const override;

    std::uint8_t rs{};
    std::uint8_t rt{};
    std::uint8_t rd{};
    std::uint8_t shiftAmount{};
    std::uint8_t functionCode{};
};

class ITypeInstruction : public Instruction {
public:
    InstructionFormat format() const override;

    std::uint8_t rs{};
    std::uint8_t rt{};
    // Raw 16 bits; signed/unsigned interpretation depends on the operation.
    std::uint16_t immediate{};
};

class JTypeInstruction : public Instruction {
public:
    InstructionFormat format() const override;

    // Raw 26-bit target field, before conversion to a byte address.
    std::uint32_t target{};
};

} // namespace mips
