// Assigned to: Zunain Nazir (CPU coordination and execution loop).
// Component implementations remain assigned to their respective owners.
#pragma once

#include "mips/alu.h"
#include "mips/control.h"
#include "mips/decoder.h"
#include "mips/memory.h"
#include "mips/program_counter.h"
#include "mips/registers.h"

#include <cstdint>
#include <ostream>
#include <vector>

namespace mips {

class CPU {
public:
    void loadProgram(const std::vector<std::uint32_t>& instructions);
    void reset();
    void step();
    void run();
    // Assigned to: all team members for their component's state display.
    void displayState(std::ostream& output) const;

private:
    // Assigned to: Timothy Sparks.
    InstructionDecoder decoder_;

    // Assigned to: Tony McFarlan.
    RegisterFile registers_;
    ALU alu_;

    // Assigned to: Haddon Stauffer.
    Memory dataMemory_;

    // Assigned to: Zunain Nazir.
    ProgramCounter programCounter_;
    ControlUnit control_;
    std::vector<std::uint32_t> instructionMemory_;
};

} // namespace mips
