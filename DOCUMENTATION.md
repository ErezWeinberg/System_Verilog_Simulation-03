# Technical Documentation

## Detailed RISC-V Multicycle Processor Implementation

This document provides in-depth technical documentation for the RISC-V multicycle processor implementation.

---

## Table of Contents

1. [Architecture Details](#architecture-details)
2. [Control Signals](#control-signals)
3. [State Machine](#state-machine)
4. [Instruction Formats](#instruction-formats)
5. [ALU Operations](#alu-operations)
6. [Memory Interface](#memory-interface)
7. [Timing Diagrams](#timing-diagrams)
8. [Design Decisions](#design-decisions)

---

## Architecture Details

### Datapath Components

#### Program Counter (PC)
- **Width**: 32 bits
- **Update**: On `pcwrite` signal
- **Sources**:
  - `PC + 4` (sequential execution)
  - `ALUOut` (branches and jumps)

#### PC Copy (PCC)
- **Purpose**: Store the current PC value for use in subsequent cycles
- **Update**: On `pccen` signal
- **Usage**: Used for computing branch targets and saving return addresses

#### Instruction Register (IR)
- **Width**: 32 bits
- **Update**: On `irwrite` signal
- **Source**: Instruction memory output

#### Register File
- **Configuration**: 32 registers × 32 bits
- **Special Register**: x0 is hardwired to 0
- **Read Ports**: 2 (registers A and B)
- **Write Port**: 1
- **Write Enable**: `regwen` signal

#### ALU
- **Width**: 32 bits
- **Input A Sources** (selected by `asel`):
  - `ALUA_PCC` (2'b00): PC Copy register
  - `ALUA_REG` (2'b01): Register file output A
  - `ALU_OUT` (2'b10): ALU output register (feedback)

- **Input B Sources** (selected by `bsel`):
  - `ALUB_IMM` (2'b00): Immediate value
  - `ALUB_REG` (2'b01): Register file output B
  - `FFFFFFF` (2'b10): Constant 0xFFFFFFFF

#### Memory Data Register (MDR)
- **Width**: 32 bits
- **Update**: On `mdrwrite` signal
- **Source**: Data memory output

#### ALU Output Register
- **Width**: 32 bits
- **Update**: Every cycle
- **Purpose**: Hold ALU result for use in subsequent cycles

---

## Control Signals

### Detailed Signal Descriptions

| Signal | Width | Direction | Description |
|--------|-------|-----------|-------------|
| `pcsourse` | 1 | DP Input | PC source select: 0=PC+4, 1=ALUOut |
| `pcwrite` | 1 | DP Input | Enable PC update |
| `pccen` | 1 | DP Input | Enable PCC (PC Copy) update |
| `irwrite` | 1 | DP Input | Enable IR (Instruction Register) update |
| `wbsel` | 2 | DP Input | Writeback source: 00=MDR, 01=ALUOut, 10=PC |
| `regwen` | 1 | DP Input | Register file write enable |
| `immsel` | 2 | DP Input | Immediate format: 00=B, 01=J, 10=S, 11=L(I) |
| `asel` | 2 | DP Input | ALU input A select |
| `bsel` | 2 | DP Input | ALU input B select |
| `alusel` | 4 | DP Input | ALU operation select |
| `mdrwrite` | 1 | DP Input | Enable MDR update |
| `memrw` | 1 | Memory Output | Memory write enable (0=read, 1=write) |
| `zero` | 1 | DP Output | ALU zero flag (result == 0) |
| `instr` | 32 | DP Output | Current instruction (from IR) |

---

## State Machine

### State Definitions

The control unit implements a 11-state FSM:

```
typedef enum {
    FETCH       = 0,   // Fetch instruction from memory
    DECODE      = 1,   // Decode instruction and prepare
    LSW_ADDR    = 2,   // Calculate load/store address
    LW_MEM      = 3,   // Read memory for load
    LW_WB       = 4,   // Writeback loaded data
    SW_MEM      = 5,   // Write memory for store
    RTYPE_ALU   = 6,   // Execute R-type ALU operation
    RTYPE_WB    = 7,   // Writeback ALU result
    BEQ_EXEC    = 8,   // Execute branch equal
    JAL_EXEC    = 9,   // Execute jump and link
    XOR_ADDI    = 10   // Special ADDI with XOR operation
} sm_type;
```

### State Transitions

```
FETCH → DECODE → {LW/SW path, ALU path, Branch path, JAL path}

Load Word (LW):
  FETCH → DECODE → LSW_ADDR → LW_MEM → LW_WB → FETCH

Store Word (SW):
  FETCH → DECODE → LSW_ADDR → SW_MEM → FETCH

R-Type ALU (ADD, SUB, etc.):
  FETCH → DECODE → RTYPE_ALU → RTYPE_WB → FETCH

ADDI (custom implementation):
  FETCH → DECODE → LSW_ADDR → XOR_ADDI → RTYPE_WB → FETCH

Branch Equal (BEQ):
  FETCH → DECODE → BEQ_EXEC → FETCH

Jump and Link (JAL):
  FETCH → DECODE → JAL_EXEC → FETCH
```

### State Details

#### FETCH State
**Purpose**: Fetch next instruction from instruction memory

**Control Signals**:
- `pccen = 1`: Save current PC to PCC
- `pcwrite = 1`: Update PC
- `irwrite = 1`: Load instruction into IR
- `pcsourse = PC_INC`: Use PC+4 as next PC

**Cycle Count**: 1

---

#### DECODE State
**Purpose**: Decode instruction and compute potential branch target

**Control Signals**:
- `immsel = IMM_B`: Use B-format immediate
- `asel = ALUA_PCC`: ALU input A = PCC
- `bsel = ALUB_IMM`: ALU input B = Immediate
- `alusel = ALU_ADD`: ALU operation = ADD

**Operation**: Computes `PCC + Branch_Offset` in case branch is taken

**Cycle Count**: 1

---

#### LSW_ADDR State
**Purpose**: Calculate effective address for load/store operations

**Control Signals**:
- `immsel = IMM_S` (for SW) or `IMM_L` (for LW/ADDI)
- `asel = ALUA_REG`: ALU input A = Register rs1
- `bsel = ALUB_IMM`: ALU input B = Immediate
- `alusel = ALU_ADD`: ALU operation = ADD

**Operation**: Computes `rs1 + offset`

**Cycle Count**: 1

---

#### XOR_ADDI State
**Purpose**: Custom operation for ADDI instruction

**Control Signals**:
- `asel = ALU_OUT`: ALU input A = Previous ALU result
- `bsel = FFFFFFF`: ALU input B = 0xFFFFFFFF
- `alusel = ALU_XOR`: ALU operation = XOR

**Operation**: XORs the result of `(rs1 + imm)` with 0xFFFFFFFF

**Cycle Count**: 1

---

#### LW_MEM State
**Purpose**: Read data from memory

**Control Signals**:
- `mdrwrite = 1`: Load data from memory into MDR

**Operation**: `MDR ← Memory[ALUOut]`

**Cycle Count**: 1

---

#### LW_WB State
**Purpose**: Write loaded data to register file

**Control Signals**:
- `wbsel = WB_MDR`: Writeback source = MDR
- `regwen = 1`: Enable register write

**Operation**: `rf[rd] ← MDR`

**Cycle Count**: 1

---

#### SW_MEM State
**Purpose**: Write data to memory

**Control Signals**:
- `memrw = 1`: Memory write enable

**Operation**: `Memory[ALUOut] ← Register rs2`

**Cycle Count**: 1

---

#### RTYPE_ALU State
**Purpose**: Execute R-type ALU operation

**Control Signals**:
- `asel = ALUA_REG`: ALU input A = Register rs1
- `bsel = ALUB_REG`: ALU input B = Register rs2
- `alusel = {funct3, inst[30]}`: ALU operation from instruction

**Operation**: `ALUOut ← rs1 OP rs2`

**Cycle Count**: 1

---

#### RTYPE_WB State
**Purpose**: Write ALU result to register file

**Control Signals**:
- `wbsel = WB_ALUOUT`: Writeback source = ALUOut
- `regwen = 1`: Enable register write

**Operation**: `rf[rd] ← ALUOut`

**Cycle Count**: 1

---

#### BEQ_EXEC State
**Purpose**: Execute conditional branch

**Control Signals**:
- `asel = ALUA_REG`: ALU input A = Register rs1
- `bsel = ALUB_REG`: ALU input B = Register rs2
- `alusel = ALU_SUB`: ALU operation = SUB
- `pcsourse = PC_ALU`: Use ALU result as PC source
- `pcwrite = zero`: Update PC only if zero flag is set

**Operation**: If `rs1 == rs2`, then `PC ← PCC + offset` (computed in DECODE)

**Cycle Count**: 1

---

#### JAL_EXEC State
**Purpose**: Execute unconditional jump and link

**Control Signals**:
- `asel = ALUA_PCC`: ALU input A = PCC
- `bsel = ALUB_IMM`: ALU input B = Immediate
- `alusel = ALU_ADD`: ALU operation = ADD
- `pcsourse = PC_ALU`: Use ALU result as PC source
- `pcwrite = 1`: Update PC
- `regwen = 1`: Enable register write
- `wbsel = WB_PC`: Writeback source = PC (return address)

**Operation**: 
- `PC ← PCC + offset`
- `rf[rd] ← PC` (save return address)

**Cycle Count**: 1

---

## Instruction Formats

### R-Type (Register-Register ALU Operations)
```
31        25 24    20 19    15 14  12 11     7 6      0
[  funct7  ] [  rs2  ] [  rs1  ][funct3][  rd   ][opcode]
```

**Encoding**: opcode = 0110011

**Supported Operations**:
- `funct3=000, funct7[5]=0`: ADD
- `funct3=000, funct7[5]=1`: SUB
- `funct3=001`: SLL (shift left logical)
- `funct3=010`: SLT (set less than)
- `funct3=011`: SLTU (set less than unsigned)
- `funct3=100`: XOR
- `funct3=101, funct7[5]=0`: SRL (shift right logical)
- `funct3=101, funct7[5]=1`: SRA (shift right arithmetic)
- `funct3=110`: OR
- `funct3=111`: AND

---

### I-Type (Immediate ALU Operations and Load)
```
31              20 19    15 14  12 11     7 6      0
[    imm[11:0]   ] [  rs1  ][funct3][  rd   ][opcode]
```

**Load Word**: opcode = 0000011, funct3 = 010
**ADDI**: opcode = 0010011, funct3 = 000

---

### S-Type (Store)
```
31        25 24    20 19    15 14  12 11     7 6      0
[imm[11:5]] [  rs2  ] [  rs1  ][funct3][imm[4:0]][opcode]
```

**Store Word**: opcode = 0100011, funct3 = 010

---

### B-Type (Branch)
```
31   30      25 24    20 19    15 14  12 11    8 7    6      0
[imm[12|10:5]] [  rs2  ] [  rs1  ][funct3][imm[4:1|11]][opcode]
```

**Branch Equal**: opcode = 1100011, funct3 = 000

---

### J-Type (Jump)
```
31                    12 11     7 6      0
[    imm[20|10:1|11|19:12] ][  rd   ][opcode]
```

**Jump and Link**: opcode = 1101111

---

## ALU Operations

### ALU Operation Encoding

The `alusel` signal is 4 bits: `{funct3, modifier}`

```systemverilog
localparam
    ALU_ADD  = 4'b0000,  // Addition
    ALU_SUB  = 4'b0001,  // Subtraction
    ALU_SLL  = 4'b0010,  // Shift left logical
    ALU_SLT  = 4'b0100,  // Set less than (signed)
    ALU_SLTU = 4'b0110,  // Set less than unsigned
    ALU_XOR  = 4'b1000,  // Bitwise XOR
    ALU_SRL  = 4'b1010,  // Shift right logical
    ALU_SRA  = 4'b1011,  // Shift right arithmetic
    ALU_OR   = 4'b1100,  // Bitwise OR
    ALU_AND  = 4'b1110;  // Bitwise AND
```

### ALU Implementation

```systemverilog
always_comb
    case (alusel)
        ALU_ADD:  alu_result = alu_a + alu_b;
        ALU_SUB:  alu_result = alu_a - alu_b;
        ALU_SLL:  alu_result = alu_a << alu_b;
        ALU_SLT:  alu_result = (alu_as < alu_bs) ? 1 : 0;  // Signed
        ALU_SLTU: alu_result = (alu_a < alu_b) ? 1 : 0;    // Unsigned
        ALU_XOR:  alu_result = alu_a ^ alu_b;
        ALU_SRL:  alu_result = alu_a >> alu_b;             // Logical
        ALU_SRA:  alu_result = alu_a >>> alu_b;            // Arithmetic
        ALU_OR:   alu_result = alu_a | alu_b;
        ALU_AND:  alu_result = alu_a & alu_b;
        default:  alu_result = alu_a + alu_b;
    endcase
```

---

## Memory Interface

### Instruction Memory Interface

```systemverilog
output logic [31:0] imem_addr;     // Address to instruction memory
input  logic [31:0] imem_datain;   // Instruction from memory
```

**Properties**:
- Read-only from processor perspective
- Addressed by PC
- Word-aligned access only
- Synchronous read (data available next cycle)

---

### Data Memory Interface

```systemverilog
output logic [31:0] dmem_addr;     // Address to data memory
output logic [31:0] dmem_dataout;  // Data to write to memory
input  logic [31:0] dmem_datain;   // Data read from memory
output logic        memrw;         // Read(0) / Write(1) control
```

**Properties**:
- Addressed by ALUOut
- Word-aligned access only
- Write data comes from register B
- Synchronous read/write (data available next cycle)

---

## Timing Diagrams

### Load Word (LW) Instruction

```
Cycle: |   1   |   2   |   3   |   4   |   5   |   6   |
State: | FETCH |DECODE |LSW_ADDR|LW_MEM | LW_WB | FETCH |
------------------------------------------------------------
PC:    | X     | X+4   | X+4   | X+4   | X+4   | X+8   |
IR:    | -     | LW    | LW    | LW    | LW    | Next  |
ALU:   | X+4   |PCC+off| rs1+imm| -    | -     | -     |
MDR:   | -     | -     | -     | Mem[A]| Mem[A]| -     |
RF:    | -     | -     | -     | -     | rd←MDR| -     |
```

### R-Type ALU Instruction (e.g., ADD)

```
Cycle: |   1   |   2   |   3   |   4   |   5   |
State: | FETCH |DECODE |RTYPE_ALU|RTYPE_WB|FETCH |
------------------------------------------------------
PC:    | X     | X+4   | X+4   | X+4   | X+8   |
IR:    | -     | ADD   | ADD   | ADD   | Next  |
ALU:   | X+4   |PCC+off|rs1+rs2|rs1+rs2| -     |
RF:    | -     | -     | -     | rd←ALU| -     |
```

### Custom ADDI Instruction

```
Cycle: |   1   |   2   |   3   |   4   |   5   |   6   |
State: | FETCH |DECODE |LSW_ADDR|XOR_ADDI|RTYPE_WB|FETCH |
------------------------------------------------------------
PC:    | X     | X+4   | X+4   | X+4   | X+4   | X+8   |
IR:    | -     | ADDI  | ADDI  | ADDI  | ADDI  | Next  |
ALU:   | X+4   |PCC+off|rs1+imm|(rs1+imm)^FFF|(...)| -  |
RF:    | -     | -     | -     | -     | rd←ALU| -     |
```

---

## Design Decisions

### 1. Custom ADDI Implementation

**Decision**: ADDI performs `(rs1 + imm) XOR 0xFFFFFFFF` instead of standard `rs1 + imm`

**Rationale**: Educational modification to demonstrate:
- Multi-cycle ALU operations
- State machine flexibility
- Custom instruction implementation

**Implementation**: Uses an extra state (XOR_ADDI) after address calculation

---

### 2. Multicycle vs. Single-Cycle

**Decision**: Multicycle architecture chosen over single-cycle

**Advantages**:
- Lower critical path (higher clock frequency potential)
- Resource sharing (one ALU for all operations)
- Realistic model of pipelined processors
- Better for educational purposes

**Disadvantages**:
- Lower throughput per instruction
- More complex control logic

---

### 3. Separate Instruction and Data Memories

**Decision**: Harvard architecture with separate memories

**Rationale**:
- Simplifies pipeline structure
- Allows simultaneous instruction fetch and data access
- Common in embedded systems and early RISC designs

---

### 4. Limited Instruction Set

**Decision**: Support only essential RISC-V instructions

**Rationale**:
- Focuses on core concepts
- Simplifies control logic
- Sufficient for demonstrating processor operation
- Extensible design for future additions

---

### 5. Word-Aligned Memory Access Only

**Decision**: No byte or halfword access support

**Rationale**:
- Simplifies memory interface
- Reduces complexity of load/store logic
- Sufficient for course objectives

**Limitation**: Cannot access individual bytes within words

---

## Register File Implementation

### Special Considerations

#### Register x0
```systemverilog
// Write
if (regwen && (addrd != 0))
    rf[addrd] <= datad;

// Read
a <= (addra == 0) ? 0 : rf[addra];
b <= (addrb == 0) ? 0 : rf[addrb];
```

**Implementation**: x0 is not physically stored; always reads as 0

---

## Immediate Generation

### Immediate Formats

The processor supports four immediate formats:

#### B-Format (Branch)
```systemverilog
imm = {{20{ir[31]}}, ir[7], ir[30:25], ir[11:8], 1'b0};
```
**Range**: ±4KB (±2048 instructions)

#### J-Format (Jump)
```systemverilog
imm = {{12{ir[31]}}, ir[19:12], ir[20], ir[30:21], 1'b0};
```
**Range**: ±1MB (±524288 instructions)

#### S-Format (Store)
```systemverilog
imm = {{21{ir[31]}}, ir[30:25], ir[11:7]};
```
**Range**: ±2KB

#### I/L-Format (Load/Immediate ALU)
```systemverilog
imm = {{21{ir[31]}}, ir[30:20]};
```
**Range**: ±2KB

All immediates are sign-extended to 32 bits.

---

## Simulation Details

### Memory Initialization

**Instruction Memory** (`imem.hex`):
- Hexadecimal format, one instruction per line
- 32-bit words (8 hex digits)
- Loaded at simulation start using `$readmemh`

**Data Memory** (`dmem_init.hex`):
- Hexadecimal format, one word per line
- Used for initial data values
- Loaded at simulation start using `$readmemh`

### Completion Detection

The simulation monitors for a specific write sequence to determine when the program has completed:

```systemverilog
if (memrw && (dmem_dataout == 32'hDEAD) && (dmem_addr == 32'hFFFF))
```

**Completion Sequence**:
1. Load completion address pointer: `lw t4, 0(x0)` → t4 = 0xFF00
2. Load completion value: `lw t5, 4(x0)` → t5 = 0xDEAD
3. Write to completion address: `sw t5, 0xFF(t4)` → Memory[0xFFFF] = 0xDEAD

### Monitoring Output

The simulation produces informational messages:
```
INFO: *** New command start: PC = 00000000, IR = 00802283
INFO: *** New command start: PC = 00000004, IR = 00C28313
...
INFO: Detected completion write sequence. Exiting.
```

---

## Testing and Verification

### Test Program Analysis

The included `test.s` program tests:
1. **Load operation**: Reading from data memory
2. **ADDI operation**: Custom implementation with XOR
3. **Store operation**: Writing to data memory
4. **R-type operations**: ADD
5. **Branch operations**: BEQ
6. **Completion sequence**: Proper simulation termination

### Expected Results

After simulation completion, `dmem_out.hex` should contain:
- Address 0x10: Result of ADDI operation
- Address 0xFFFF: 0xDEAD (completion marker)

---

## Extension Possibilities

### Additional Instructions
- **ANDI, ORI, XORI**: Immediate logical operations
- **SLTI, SLTIU**: Immediate comparison
- **LUI, AUIPC**: Upper immediate operations
- **BNE, BLT, BGE**: Additional branch types
- **JALR**: Indirect jump

### Advanced Features
- **Byte/Halfword Access**: LB, LBU, LH, LHU, SB, SH
- **Multiply/Divide**: M extension
- **CSR Instructions**: System and counter access
- **Exception Handling**: Interrupts and traps

### Performance Enhancements
- **Pipeline Forwarding**: Reduce data hazards
- **Branch Prediction**: Reduce branch penalties
- **Cache**: Add instruction and data caches

---

## References

- **RISC-V ISA Specification**: https://riscv.org/specifications/
- **Patterson & Hennessy**: "Computer Organization and Design: The Hardware/Software Interface"
- **Harris & Harris**: "Digital Design and Computer Architecture: RISC-V Edition"

---

## Appendix: Quick Reference

### Control Signal Values by State

| State | pcwrite | pccen | irwrite | regwen | memrw | mdrwrite |
|-------|---------|-------|---------|--------|-------|----------|
| FETCH | 1 | 1 | 1 | 0 | 0 | 0 |
| DECODE | 0 | 0 | 0 | 0 | 0 | 0 |
| LSW_ADDR | 0 | 0 | 0 | 0 | 0 | 0 |
| LW_MEM | 0 | 0 | 0 | 0 | 0 | 1 |
| LW_WB | 0 | 0 | 0 | 1 | 0 | 0 |
| SW_MEM | 0 | 0 | 0 | 0 | 1 | 0 |
| RTYPE_ALU | 0 | 0 | 0 | 0 | 0 | 0 |
| RTYPE_WB | 0 | 0 | 0 | 1 | 0 | 0 |
| BEQ_EXEC | zero | 0 | 0 | 0 | 0 | 0 |
| JAL_EXEC | 1 | 0 | 0 | 1 | 0 | 0 |
| XOR_ADDI | 0 | 0 | 0 | 0 | 0 | 0 |

### Instruction Cycle Counts

| Instruction | Cycles | States |
|-------------|--------|--------|
| LW | 6 | FETCH → DECODE → LSW_ADDR → LW_MEM → LW_WB → FETCH |
| SW | 5 | FETCH → DECODE → LSW_ADDR → SW_MEM → FETCH |
| R-Type | 5 | FETCH → DECODE → RTYPE_ALU → RTYPE_WB → FETCH |
| ADDI | 6 | FETCH → DECODE → LSW_ADDR → XOR_ADDI → RTYPE_WB → FETCH |
| BEQ | 4 | FETCH → DECODE → BEQ_EXEC → FETCH |
| JAL | 4 | FETCH → DECODE → JAL_EXEC → FETCH |

---

*This documentation is part of the Technion EE 044252 Digital Systems and Computer Structure course.*
