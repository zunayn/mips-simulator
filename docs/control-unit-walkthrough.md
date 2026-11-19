# Zunain's next component: the control unit

The program counter stores an address. The control unit decides what an instruction
requires, including which address should come next. CPU uses those decisions to
coordinate the registers, ALU, memory, and program counter.

This milestone implements `ControlUnit` in `include/mips/control.h` and
`src/control.cpp`, following the structure and method signatures on `main`. It uses
the working branch's `ALUOp` enum instead of main's proposed `ALUOperation` enum.
The R/I/J instruction classes also come from main's interface; their `format()`
methods simply identify their layouts. The existing machine-word decoder remains
R-type-only, with a small conversion in CPU to the shared R-type class.

## 1. Start with one instruction

For `add $t0, $t1, $t2`, three different questions have different owners:

| Question | Component that answers it |
|---|---|
| Which register numbers and function code are encoded in this word? | Instruction decoder |
| Which ALU operation and writeback path should be selected? | Control unit |
| What are the input values, and how do we carry out the selected actions? | CPU using registers and ALU |

The control unit does not perform `10 + 20` itself. It returns a `ControlSignals`
object saying to select Add, enable register writing, and use `rd` as the
destination. CPU then reads the operands, calls the ALU, and writes the result.

## 2. Read `decode()` before `nextAddress()`

`ControlUnit::decode` starts with all booleans false. This means no write, load,
store, branch, or jump is accidentally enabled. It then recognizes the instruction
and turns on only the required actions.

For an R-type `add`, the important decisions are:

```text
aluOperation = ADD         choose addition
registerWrite = true      store the result in a register
destinationIsRd = true    use the instruction's rd field
aluSourceImmediate = false use the second register value, not an immediate
memoryRead/Write = false  do not access data memory
branch/jump = false      take the ordinary sequential path
```

The boolean names describe hardware routing choices. A circuit uses a multiplexer
to select between inputs; your C++ implementation uses conditions. In this
milestone, CPU reads `signals.aluOperation`, `signals.registerWrite`, and
`signals.destinationIsRd` while executing its R-type instructions.

The same method can already describe other instructions in component tests:

| Instruction | What its signals request |
|---|---|
| `addi`, `andi`, `ori` | Use an immediate operand and write the result into `rt` |
| `lw` | Calculate an address, read memory, and write that value into `rt` |
| `sw` | Calculate an address and write memory; leave registers unchanged |
| `beq`, `bne` | Enable the conditional-branch path; do not write registers |
| `j` | Enable the jump path; do not write registers |
| `jal` | Enable the jump path and request a link-register write |

Our existing ALU distinguishes ADD (signed overflow is an error) from ADDU
(wrapping arithmetic). Load/store address generation selects ADDU. Branch signals
select SUBU if a future datapath uses subtraction for equality, while the current
`nextAddress` implementation compares the values directly. Neither approach should
produce a signed-subtraction overflow for a branch comparison.

Control signals are requests, not proof that a state change happened. For example,
`nop` is an R-type shift targeting `$zero`. Control enables register writing, but
RegisterFile ignores writes to register zero. The trace distinguishes the control
write-enable from the actual register-write indicator.

## 3. Calculate the next address without changing state

`nextAddress(currentAddress, instruction, registers)` returns a number. It does
not change the real PC or any register. This lets CPU validate and compute results
before committing changes.

For an ordinary R-type instruction at `0x00400000`, it returns `0x00400004`.
It uses a local ProgramCounter to enforce alignment and the feature branch's
existing PC-overflow check. An invalid sequential PC advance throws an exception.

For a branch, first compare the register values. BEQ takes the branch when equal;
BNE takes it when different. An untaken branch still returns PC + 4. A taken
branch uses:

```text
target = (PC + 4) + signedImmediate * 4
```

Example: at address `0x00400008`, an offset of -3 targets `0x00400000`:
the next sequential address is `0x0040000C`, and subtracting 12 bytes gives the
target. The immediate counts instructions, so multiplying by four converts it
to bytes. A negative immediate permits a backward branch for a loop.

The field stores raw 16-bit bits. `0xFFFD` represents -3 when interpreted as
signed. The implementation subtracts 65,536 when the sign bit is set, then uses
a wide signed intermediate for the calculation. Casting the final result to
`uint32_t` keeps the low 32 address bits. Whether that resulting address belongs
to the loaded program is a separate instruction-memory/fetch concern.

For a jump, the 26-bit target field represents address bits 27–2. Shift it left
two places and combine it with the upper four bits of PC + 4. It is not a branch
offset. A `jal` uses the same target calculation as `j`; its additional `link`
signal requests that CPU save PC + 4 into register 31 when that path is integrated.
This design has no delay slots.

## 4. What is connected today?

Run these commands from the repository root:

```sh
make run
make test
```

The demo's R-type instructions now print a `CONTROL` row. For ADD you should see
`RegWrite=1`, `RegDst=1`, `ALUSrc=0`, and the memory/branch/jump flags at zero.
The existing CPU results and stopping behavior remain the same.

`tests/test_control.cpp` constructs I/J instruction objects directly. Those tests
check both branch outcomes, negative offsets, immediate boundaries, jump-address
construction, link signals, and invalid fields. They do not execute I/J machine
words through CPU. Those words are still rejected by the R-type decoder.

## 5. How to study this implementation

1. Read the `ControlSignals` fields in the header. Explain what a true value would
   ask the CPU to do before looking at the implementation.
2. Follow the R-type case of `decode()` and compare it with the demo's CONTROL row.
3. Read the BEQ/BNE cases and the `signals.branch` block in `nextAddress()`.
   Work out the target for a branch at byte address 12 with immediate -3: it is 4.
4. Find those calculations in `tests/test_control.cpp`. Change the operands on
   paper first and predict whether the next address is sequential or a target.
5. Follow the use of `control_` in `SingleCycleCPU::stepRType`. Notice that CPU
   writes registers and PC only after validation and ALU calculation succeed.

The next task is CPU integration of `beq`, `bne`, `j`, and `jal`: extend decoding
to supply the shared I/J objects, execute the control-flow paths without an
unwanted ALU writeback, save the `jal` return address, and display truthful traces.
That work will turn the independently tested decisions into executing programs.
