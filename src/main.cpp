#include "cpu.hpp"
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    try {
        SingleCycleCPU cpu;
        cpu.registers.write(9, 10);   // $t1 = 10
        cpu.registers.write(10, 20);  // $t2 = 20
        if (argc == 1) {
            // Five sequential R-type instructions, with data dependencies.
            cpu.loadProgram({
                encodeRType(9, 10, 8, 0, 0x20),  // add  $t0,$t1,$t2 = 30
                encodeRType(8, 9, 11, 0, 0x22),  // sub  $t3,$t0,$t1 = 20
                encodeRType(8, 11, 12, 0, 0x24), // and  $t4,$t0,$t3 = 20
                encodeRType(0, 11, 13, 2, 0x00), // sll  $t5,$t3,2 = 80
                0u                               // nop
            });
        } else if (argc == 3 && std::string(argv[1]) == "--file") {
            cpu.loadProgramFile(argv[2]);
        } else if (argc == 3 && std::string(argv[1]) == "--word") {
            const std::string input(argv[2]);
            std::size_t consumed = 0;
            const unsigned long long parsed = std::stoull(input, &consumed, 16);
            if (consumed != input.size() || parsed > UINT32_MAX)
                throw std::invalid_argument("Expected a 32-bit hexadecimal word");
            cpu.loadProgram({static_cast<uint32_t>(parsed)});
        } else {
            throw std::invalid_argument("Usage: ./demo [--file examples/program.hex | --word 0x012A4020]");
        }
        std::cout << "Single-cycle MIPS: sequential instruction-memory demo\n";
        const auto traces = cpu.run();
        for (std::size_t i = 0; i < traces.size(); ++i)
            std::cout << "--- Instruction " << i+1 << " (one simulated clock cycle) ---\n"
                      << formatTrace(traces[i]);
        std::cout << "HALT: PC reached end of loaded program (simulator convention, not MIPS halt).\n"
                  << "Final registers: $t0=" << cpu.registers.read(8)
                  << " $t3=" << cpu.registers.read(11)
                  << " $t4=" << cpu.registers.read(12)
                  << " $t5=" << cpu.registers.read(13) << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Stopped: " << error.what() << '\n';
        return 1;
    }
}
