# CS3339 MIPS simulator - starter setup

This is a **new local scaffold**, NOT source code currently present in the team's repository. It implements only the proposed Register File and ALU workstream (Tony), not a complete CPU simulator.

## Requirements

- Git and VS Code (optional)
- C++17 compiler (`g++` or `clang++`) and `make`
- MARS (optional, for independently running the `.asm` example)

## Quick start (macOS/Linux)

```bash
make
./demo
make test
```

Expected demo result: `$t0 = 30`. Expected tests: `All register and ALU tests passed.`

On Windows, use WSL with g++ and make, or adapt the Makefile for MSYS2/MinGW.

## Working with the team repository

```bash
git clone https://github.com/zunayn/mips-simulator.git
cd mips-simulator
git switch -c feature/registers-alu
```

Copy the contents of this starter folder into the clone, excluding the starter folder itself. Keep the existing team's README.md. Then:

```bash
make test
git add include src tests examples Makefile .vscode SETUP.md
git commit -m "Add register file and ALU starter implementation"
# Only push if you have repository write access and team agreement:
git push -u origin feature/registers-alu
```

## Design/integration notes

- Registers use 32-bit values, with `$zero` (register 0) write-protected.
- ADD/SUB use signed MIPS32 semantics and detect signed overflow.
- AND/OR use bitwise semantics; SLT compares signed values.
- `executeRType` assumes the parser already decoded `rd`, `rs`, `rt` and operation.
- Team integration remains: instruction parser/decoder, memory, PC/control flow, CPU execute loop, instruction file loading, and end-to-end tests.
- `examples/alu_demo.asm` is a separate reference program for MARS; the scaffold cannot yet read `.asm` files.
