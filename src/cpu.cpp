#include "mips/cpu.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

// Assigned to: Zunain Nazir (program-counter integration).
SingleCycleCPU::SingleCycleCPU() {
    programCounter_.set(InstructionMemory::kTextBase);
}

uint32_t SingleCycleCPU::pc() const {
    return programCounter_.value();
}

const char* aluOpName(ALUOp op) {
    switch (op) {
        case ALUOp::ADD: return "ADD";
        case ALUOp::ADDU: return "ADDU";
        case ALUOp::SUB: return "SUB";
        case ALUOp::SUBU: return "SUBU";
        case ALUOp::AND: return "AND";
        case ALUOp::OR: return "OR";
        case ALUOp::XOR: return "XOR";
        case ALUOp::NOR: return "NOR";
        case ALUOp::SLT: return "SLT";
        case ALUOp::SLTU: return "SLTU";
        case ALUOp::SLL: return "SLL";
        case ALUOp::SRL: return "SRL";
        case ALUOp::SRA: return "SRA";
    }
    throw std::invalid_argument("Unknown ALU op");
}

std::string formatRType(const DecodedRType& d, ALUOp op) {
    std::ostringstream s;
    if (d.raw == 0) return "nop"; // sll $zero,$zero,0
    std::string name = aluOpName(op);
    for (char& c : name) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + ('a' - 'A'));
    if (op == ALUOp::SLL || op == ALUOp::SRL || op == ALUOp::SRA)
        s << name << " $" << d.rd << ", $" << d.rt << ", " << d.shamt;
    else
        s << name << " $" << d.rd << ", $" << d.rs << ", $" << d.rt;
    return s.str();
}

ExecutionTrace SingleCycleCPU::stepRType(uint32_t word) {
    const DecodedRType d = decodeRType(word);
    // Bridge the feature branch's decoder to main's shared instruction interface.
    mips::RTypeInstruction instruction;
    instruction.opcode = static_cast<std::uint8_t>(d.opcode);
    instruction.rs = static_cast<std::uint8_t>(d.rs);
    instruction.rt = static_cast<std::uint8_t>(d.rt);
    instruction.rd = static_cast<std::uint8_t>(d.rd);
    instruction.shiftAmount = static_cast<std::uint8_t>(d.shamt);
    instruction.functionCode = static_cast<std::uint8_t>(d.funct);
    const auto signals = control_.decode(instruction);
    const ALUOp op = signals.aluOperation;
    const uint32_t oldPC = programCounter_.value();
    const uint32_t nextPC = control_.nextAddress(oldPC, instruction, registers);
    const uint32_t lhs = registers.read(d.rs);
    const uint32_t rhs = registers.read(d.rt);
    // No state changes until all validation and ALU work succeed.
    const ALUResult value = aluEvaluate(op, lhs, rhs, d.shamt);
    const unsigned destination = signals.destinationIsRd ? d.rd : d.rt;
    if (signals.registerWrite) registers.write(destination, value.value);
    programCounter_.set(nextPC);
    return {oldPC, programCounter_.value(), d, op, lhs, rhs, value.value,
            value.zero, signals.registerWrite && destination != 0, formatRType(d, op), signals};
}

std::string formatTrace(const ExecutionTrace& t) {
    std::ostringstream s;
    s << "FETCH       PC=0x" << std::hex << std::setw(8) << std::setfill('0') << t.pcBefore
      << " word=0x" << std::setw(8) << t.decoded.raw << std::dec << '\n'
      << "DECODE      " << t.assembly << "  opcode=" << t.decoded.opcode
      << " rs=" << t.decoded.rs << " rt=" << t.decoded.rt
      << " rd=" << t.decoded.rd << " shamt=" << t.decoded.shamt
      << " funct=0x" << std::hex << t.decoded.funct << std::dec << '\n'
      << "CONTROL     RegWrite=" << t.control.registerWrite
      << " RegDst=" << t.control.destinationIsRd
      << " ALUSrc=" << t.control.aluSourceImmediate
      << " MemRead=" << t.control.memoryRead << " MemWrite=" << t.control.memoryWrite
      << " Branch=" << t.control.branch << " Jump=" << t.control.jump << '\n'
      << "READ        rs=" << t.operandA << " rt=" << t.operandB << '\n'
      << "EXECUTE     ALU=" << aluOpName(t.operation) << " result=" << t.result
      << " zero=" << t.zero << '\n'
      << "WRITE-BACK  " << (t.regWrite ? "regWrite=1" : "regWrite=0 ($zero protected)")
      << " rd=$" << t.decoded.rd << '\n'
      << "NEXT PC     0x" << std::hex << std::setw(8) << std::setfill('0') << t.pcAfter << '\n';
    return s.str();
}

void SingleCycleCPU::loadProgram(const std::vector<uint32_t>& words, uint32_t base) {
    instructions.load(words, base);
    programCounter_.set(base);
}

void SingleCycleCPU::loadProgramFile(const std::string& path, uint32_t base) {
    instructions.loadHexFile(path, base);
    programCounter_.set(base);
}

bool SingleCycleCPU::halted() const {
    return programCounter_.value() == instructions.endAddress();
}

ExecutionTrace SingleCycleCPU::step() {
    // Fetch fails without modifying registers or the PC.
    return stepRType(instructions.fetch(programCounter_.value()));
}

std::vector<ExecutionTrace> SingleCycleCPU::run(std::size_t maxInstructions) {
    std::vector<ExecutionTrace> traces;
    while (!halted()) {
        if (traces.size() == maxInstructions)
            throw std::runtime_error("Instruction limit reached before program end");
        traces.push_back(step());
    }
    return traces;
}
