# CS3339 Fall 2026 – Project Proposal - MIPS Simulator

## Team Members
- Timothy Sparks
- Tony McFarlan
- Haddon Stauffer
- Zunain Nazir

## Short Project Summary
Our project will involve developing a simulator for a MIPS processor. The simulator will accept MIPS instructions, interpret their behavior, and model important parts of processor execution such as registers, memory, instruction decoding, and execution.

Depending on the final scope of the project, the simulator may also model concepts such as the datapath, control signals, instruction cycles, or pipelined execution.

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

The simulator should be able to:

- Read or receive MIPS instructions.
- Decode supported instructions.
- Execute those instructions correctly.
- Maintain the values of MIPS registers.
- Simulate memory where necessary.
- Display changes to the processor state.
- Support multiple common MIPS instruction types.

Possible additional goals include:

- Showing instruction fields such as opcode, `rs`, `rt`, `rd`, immediate, and function code.
- Displaying control signals.
- Showing the datapath used by an instruction.
- Simulating instruction execution cycle-by-cycle.
- Supporting pipelining.
- Detecting pipeline hazards.

## Expected Results

We expect the finished simulator to correctly execute a selected subset of the MIPS instruction set and produce the same register and memory results that would be expected from actual MIPS execution.

The simulator should also provide enough information about each instruction to demonstrate how the processor handles different categories of instructions.

---

# 1.2 Methodology

## Step 1 – Determine Supported MIPS Instructions

The team will first decide which instructions the simulator must support.

Possible starting instructions:

### R-Type
- `add`
- `sub`
- `and`
- `or`
- `slt`

### I-Type
- `addi`
- `andi`
- `ori`
- `lw`
- `sw`
- `beq`
- `bne`

### J-Type
- `j`
- `jal`

Additional instructions can be added if time permits.

---

## Step 2 – Design Processor Components

The simulator will model the main processor components needed for instruction execution.

Possible components include:

- Program Counter (PC)
- Instruction Memory
- Register File
- ALU
- Data Memory
- Control Unit
- Instruction Decoder
- Immediate Generator / Sign Extension
- Branch and Jump Logic

Each component can be represented using software classes, structures, or functions.

---

## Step 3 – Instruction Parsing and Decoding

The simulator will accept MIPS instructions in one of the following forms:

- MIPS assembly instructions
- 32-bit binary instructions
- hexadecimal machine code

The exact input format will be decided during implementation.

The decoder will determine:

- Opcode
- Source registers
- Destination register
- Immediate value
- Function code
- Instruction type

---

## Step 4 – Instruction Execution

After decoding an instruction, the simulator will execute the required operation.

For example:

`add $t0, $t1, $t2`

The simulator would:

1. Read the contents of `$t1`.
2. Read the contents of `$t2`.
3. Send the values to the simulated ALU.
4. Perform addition.
5. Store the result in `$t0`.
6. Update the program counter.

For a memory instruction such as:

`lw $t0, 4($t1)`

the simulator would calculate the memory address and load the appropriate value into `$t0`.

---

## Step 5 – Processor State

The simulator will maintain the current processor state.

This may include:

- Program Counter
- 32 MIPS registers
- Memory values
- Current instruction
- ALU result
- Control signals

After each instruction, the simulator can display the updated state.

---

## Step 6 – Testing

The team will create small MIPS programs to test different instruction types.

Tests may include:

- Arithmetic operations
- Logical operations
- Register transfers
- Memory loads and stores
- Branches
- Loops
- Jump instructions

The simulator output will be compared with expected results to verify correctness.

If available, results may also be compared with an existing MIPS simulator such as MARS or QtSPIM.

---

## Tools

Possible development tools:

- Programming language: [C++ / Python / Java]
- Git / TXST GitLab
- MARS or QtSPIM for reference testing
- Command-line testing tools
- IDE/editor used by team members

---

# 1.3 Deliverables

## MIPS Simulator

The main deliverable will be a working MIPS processor simulator.

The simulator source code will be stored in the team's TXST Git repository.

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



# <span style="color:red">** (END OF MATERIAL FOR PROPOSAL DOCUMENT - INFO FOLLOWING THIS IS TO BE USED INTERNALLY WITH TEAM MEMBERS )** </span>


# 1.4 Team

## Proposed Work Division (Draft) (Every person must put their role and repsonsibilities here.)

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

# Internal Project Breakdown (Add your name to task of choice.)

## Member 1 – Instruction Decoder

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

## Member 2 – Register File and ALU

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

## Member 3 – Memory System

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

## Member 4 – Program Control

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


# Decisions the Team Needs to Make (Some questions from ChatGPT, refine and edit)

Before writing the final proposal, we need to decide:

- Which programming language we will use.
- Whether input will be assembly, binary machine code, or both. (Probably assembly)
- Which MIPS instructions will be supported.
- Whether we are simulating a single-cycle processor or something more advanced.
- Whether the simulator will show control signals.
- Whether the simulator will show the datapath.
- Whether pipelining will be included.
- How the processor state will be displayed.
- How we will test correctness. (Using another simulator like Mars to test if our simulator has the same results)
