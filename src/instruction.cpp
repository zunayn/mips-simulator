// Assigned to: Timothy Sparks (shared instruction-format interface from main).
#include "mips/instruction.h"

namespace mips {

InstructionFormat RTypeInstruction::format() const {
    return InstructionFormat::RType;
}

InstructionFormat ITypeInstruction::format() const {
    return InstructionFormat::IType;
}

InstructionFormat JTypeInstruction::format() const {
    return InstructionFormat::JType;
}

} // namespace mips
