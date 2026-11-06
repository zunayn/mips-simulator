# Team integration and Git workflow

## Who owns what

- **Your responsibility:** 32-entry register file, R-type ALU control, 13 ALU operations, Zero flag, signed-vs-unsigned cases and regression tests. Your CPU integration glue demonstrates these modules.
- **Timothy:** final decoder implementation and the decoded-field contract. Discuss whether he will also own main control.
- **Whole team:** instruction memory, PC, main-control signals, data memory, immediate/branch/jump paths, and overall testing according to assigned roles. Do not assume the reference instruction-memory scaffold is the team's final design.

## API boundary to agree on

```cpp
struct DecodedRType {
    uint32_t raw;
    unsigned opcode, rs, rt, rd, shamt, funct;
};
DecodedRType decodeRType(uint32_t word);
ALUOp aluOpFromFunct(unsigned funct);
ALUResult aluEvaluate(ALUOp op, uint32_t lhs, uint32_t rhs, unsigned shamt);
uint32_t RegisterFile::read(unsigned index) const;
void RegisterFile::write(unsigned index, uint32_t value);
```

For future I/J instructions, design a more general decoder output instead of forcing I/J encodings into `DecodedRType`.

## Safe local integration

```bash
# From the actual team repository, not from a parent starter folder:
git status
git switch -c feature/single-cycle-program-memory
# Review/copy selected source, tests, and docs into this checkout.
git diff --stat
make clean && make && make test
# When both owners approve changes:
git add include src tests examples docs Makefile SETUP.md .gitignore
git commit -m "Integrate single-cycle instruction memory and R-type datapath"
```

Avoid a blind recursive `cp` if teammates have modified the same files. Share a pull request and merge only after the decoder/control interfaces agree. Do not commit executables.

## Follow-on single-cycle work

1. Add main control (`RegDst`, `RegWrite`, `ALUSrc`, `MemRead`, `MemWrite`, `MemToReg`, `Branch`, `Jump`, `ALUOp`).
2. Extend the decoder to I/J instruction formats and immediate extension.
3. Add data memory, address alignment rules, `lw` and `sw`.
4. Add branch target calculation and PC-select mux; test `beq` and `bne`.
5. Integrate all instruction types into one processor step, and draw a complete single-cycle datapath diagram.
6. Verify instruction programs against MARS results and write final integration tests.

Only implement features your professor's rubric and team scope require. Do not add a pipeline unless the requirements change.
