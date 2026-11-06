# CS3339 Fall 2026 – Project Proposal - MIPS Simulator

## Team Members
- Timothy Sparks
- Tony McFarlan
- Haddon Stauffer
- Zunain Nazir

## Short Project Summary
Our project will involve developing a single-cycle MIPS processor simulator in C++. The simulator will accept MIPS assembly instructions, decode and execute a selected subset of the instruction set, and model important processor components such as the program counter, register file, ALU, control logic, and memory. Additional features such as datapath visualization, control-signal display, or pipelining may be explored as stretch goals if time permits.

---

# 1.1 Problem Description

## Project Problem

The goal of the project is to develop a software-based simulator that demonstrates how a MIPS processor executes instructions.

Although MIPS assembly programs show what instructions the processor receives, they do not directly show what is happening inside the processor while those instructions execute. Our simulator will help represent this process by modeling components of a MIPS processor and showing how instructions affect registers, memory, and processor state.

## Why the Project Is Useful

MIPS is commonly used to teach computer architecture because its instruction set is relatively simple and its instruction formats clearly demonstrate concepts such as:

- registers
- memory access
- arithmetic and logical operations
- instruction encoding
- control flow
- processor datapaths
- instruction execution

A simulator can make these concepts easier to understand by allowing users to observe changes in the processor state as individual instructions execute.

The project will also allow us to apply several concepts from CS3339 in a working software system instead of studying them only theoretically.

## Main Goals

The simulator will be designed as a single-cycle MIPS processor simulator and will support a selected subset of common MIPS instructions.

The core simulator will:

Accept MIPS assembly instructions as input.

Decode and execute supported instructions.

Maintain the values of the 32 MIPS registers.

Maintain and update the Program Counter (PC).

Simulate data memory for load and store instructions.

Model the ALU and basic processor control logic.

Display changes to registers, memory, and processor state after instructions are executed.

Support R-type, I-type, and J-type instructions.

The initial instruction set will include:

R-Type:

`add`

`sub`

`and`

`or`

`slt`

I-Type:

`addi`

`andi`

`ori`

`lw`

`sw`

`beq`

`bne`

J-Type:

`j`

`jal`

If the core simulator is completed successfully, additional features may be implemented as stretch goals. These include displaying control signals, visualizing the MIPS datapath, supporting additional instructions, and potentially extending the simulator to model pipelined execution and pipeline hazards.

## Expected Results

We expect the finished simulator to correctly execute the selected subset of the MIPS instruction set and produce the same register, memory, and program-counter results expected from MIPS execution.

Correctness will be tested by running the same MIPS programs in our simulator and in MARS and comparing the resulting processor state.

---

# 1.2 Methodology

The project will begin by designing the major components of a single-cycle MIPS processor in software. These components will include the Program Counter, instruction decoder, register file, ALU, data memory, and control logic. The simulator will be written in C++, with different parts of the processor separated into software components so that they can be developed and tested independently.

The simulator will accept MIPS assembly instructions as input. The instruction parser and decoder will identify the instruction type, registers, immediate values, and other information required for execution. The appropriate processor components will then perform the operation and update the Program Counter, register file, or memory as necessary.

Development will initially focus on the selected R-type, I-type, and J-type instructions listed above. Once individual instructions work correctly, they will be combined into small MIPS programs containing arithmetic operations, memory operations, branches, jumps, and loops. This will allow the team to test interactions between the different simulator components.

To verify correctness, the team will create a collection of MIPS test programs and execute them using both our simulator and MARS. The resulting register values, memory values, and control flow will be compared. A test will be considered successful when our simulator produces the same final processor state as MARS for all instructions supported by the project.

Development and Testing Tools

Programming Language: C++

Processor Model: Single-cycle MIPS processor

Input Format: MIPS assembly instructions

Reference Simulator: MARS

Version Control: Git / GitHub

Test Workloads: Custom MIPS programs containing arithmetic, logical, memory, branch, jump, and loop operations

Testing Method: Compare register, memory, and Program Counter results with MARS

Development Environment: Linux-based development environment with a C++ compiler and command-line tools

---

# 1.3 Deliverables

## MIPS Simulator

The main deliverable will be a working MIPS processor simulator.

The simulator source code will be stored in the team's GitHub repository.

Repository:

`[https://github.com/zunayn/mips-simulator/]`

## Documentation

The repository will include documentation explaining:

- How to build or run the simulator.
- Supported instructions.
- Input format.
- Program structure.
- Known limitations.

## Test Programs

The team will provide several MIPS programs that demonstrate and test the simulator.

## Final Report

The written report will explain:

- Simulator design.
- Processor components modeled.
- Supported instructions.
- Implementation decisions.
- Testing methodology.
- Results.
- Limitations.
- Possible improvements.


# 1.4 Team

## 

| Team Member | Main Responsibility | Additional Responsibility |
|---|---|---|
| Timothy Sparks  | Instruction Parser / Decoder | Instruction format research |
| Tony McFarlan   | Registers + ALU | R-type instruction implementation |
| Haddon Stauffer | Memory + Load/Store | I-type instruction implementation |
| Zunain Nazir    | Control Flow + Program Counter | Branch/jump implementation |

All members will participate in:

- Testing
- Debugging
- Code review
- Documentation
- Final report
- Presentation

---

# Assigned respsonsibilites for team members

## Timothy Sparks – Instruction Decoder

Research and implement:

- MIPS instruction formats
- Opcode decoding
- `rs`, `rt`, and `rd`
- Immediate values
- Function codes

Possible files:

`decoder.cpp`

`instruction.cpp`

---

## Tony McFarlan – Register File and ALU

Implement:

- 32 MIPS registers
- Register reads
- Register writes
- Arithmetic operations
- Logical operations

Possible files:

`registers.cpp`

`alu.cpp`

---

## Haddon Stauffer – Memory System

Implement:

- Simulated memory
- Address calculation
- `lw`
- `sw`
- Memory initialization
- Memory display/debugging

Possible files:

`memory.cpp`

---

## Zunain Nazir – Program Control

Implement:

- Program Counter
- Sequential instruction execution
- Branch instructions
- Jump instructions
- Program execution loop

Possible files:

`cpu.cpp`

`control.cpp`

---


