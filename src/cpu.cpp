#include "cpu.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>

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
    if (pc > 0xfffffffbu) throw std::overflow_error("PC + 4 overflow");
    const DecodedRType d = decodeRType(word);
    const ALUOp op = aluOpFromFunct(d.funct);
    const uint32_t lhs = registers.read(d.rs);
    const uint32_t rhs = registers.read(d.rt);
    // No state changes until all validation and ALU work succeed.
    const ALUResult value = aluEvaluate(op, lhs, rhs, d.shamt);
    const uint32_t oldPC = pc;
    registers.write(d.rd, value.value);
    pc += 4;
    return {oldPC, pc, d, op, lhs, rhs, value.value,
            value.zero, d.rd != 0, formatRType(d, op)};
}

std::string formatTrace(const ExecutionTrace& t) {
    std::ostringstream s;
    s << "FETCH       PC=0x" << std::hex << std::setw(8) << std::setfill('0') << t.pcBefore
      << " word=0x" << std::setw(8) << t.decoded.raw << std::dec << '\n'
      << "DECODE      " << t.assembly << "  opcode=" << t.decoded.opcode
      << " rs=" << t.decoded.rs << " rt=" << t.decoded.rt
      << " rd=" << t.decoded.rd << " shamt=" << t.decoded.shamt
      << " funct=0x" << std::hex << t.decoded.funct << std::dec << '\n'
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
    pc = base;
}

void SingleCycleCPU::loadProgramFile(const std::string& path, uint32_t base) {
    instructions.loadHexFile(path, base);
    pc = base;
}

bool SingleCycleCPU::halted() const {
    return pc == instructions.endAddress();
}

ExecutionTrace SingleCycleCPU::step() {
    // Fetch fails without modifying registers or the PC.
    return stepRType(instructions.fetch(pc));
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
