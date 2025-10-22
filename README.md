# RISC-V Multicycle Processor Simulation

**Technion EE 044252: Digital Systems and Computer Structure**

This project implements a simple multicycle RISC-V processor model in SystemVerilog. The design follows a classic multicycle datapath architecture with separate control and data path modules.

## Table of Contents
- [Overview](#overview)
- [Architecture](#architecture)
- [Supported Instructions](#supported-instructions)
- [File Structure](#file-structure)
- [Simulation](#simulation)
- [Memory Organization](#memory-organization)
- [How It Works](#how-it-works)
- [Example Program](#example-program)

## Overview

This is a multicycle RISC-V processor implementation designed for educational purposes. The processor executes a subset of the RISC-V instruction set and demonstrates the fundamental concepts of computer architecture including:
- Multicycle instruction execution
- Finite state machine-based control
- Register file management
- Memory hierarchy (instruction and data memory)
- ALU operations

## Architecture

The design consists of three main components:

### 1. Top Level (`rv_top.sv`)
- Integrates the datapath and control modules
- Manages the interface between processor and memory
- Handles clock and reset signals

### 2. Datapath (`rv_dp.sv`)
- **32-bit datapath** with configurable register file size
- **Register File**: 32 general-purpose registers (x0-x31), where x0 is hardwired to 0
- **ALU**: Supports arithmetic, logical, and shift operations
- **Pipeline Registers**: PC, PCC (PC Copy), IR (Instruction Register), A, B, ALUOut, MDR (Memory Data Register)
- **Immediate Generation**: Supports multiple immediate formats (B, J, S, L)

### 3. Control Unit (`rv_ctl.sv`)
Implements a finite state machine with the following states:
- **FETCH**: Fetch instruction from instruction memory
- **DECODE**: Decode the instruction and compute branch target
- **LSW_ADDR**: Calculate load/store address
- **LW_MEM**: Read from data memory (load)
- **LW_WB**: Write loaded data to register file
- **SW_MEM**: Write to data memory (store)
- **RTYPE_ALU**: Execute R-type ALU operation
- **RTYPE_WB**: Write ALU result to register file
- **BEQ_EXEC**: Execute branch equal instruction
- **JAL_EXEC**: Execute jump and link instruction
- **XOR_ADDI**: Special state for ADDI instruction with XOR operation

## Supported Instructions

The processor currently supports the following RISC-V instructions:

| Instruction | Type | Description |
|-------------|------|-------------|
| `LW` | I-type | Load word from memory |
| `SW` | S-type | Store word to memory |
| `ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND` | R-type | Register-register ALU operations |
| `ADDI` | I-type | Add immediate (with special XOR 0xFFFFFFFF operation) |
| `BEQ` | B-type | Branch if equal |
| `JAL` | J-type | Jump and link |

## File Structure

```
.
├── rv_top.sv          # Top-level module integrating datapath and control
├── rv_dp.sv           # Datapath implementation
├── rv_ctl.sv          # Control unit with FSM
├── rv_sim.sv          # Simulation testbench
├── params.inc         # Parameter definitions (opcodes, constants)
├── imem.hex           # Instruction memory initialization (hex format)
├── dmem_init.hex      # Data memory initialization (hex format)
├── test.s             # Example assembly program
└── README.md          # This file
```

## Simulation

### Prerequisites
- SystemVerilog simulator (e.g., ModelSim, Questa, VCS, or Verilator)
- Basic understanding of RISC-V assembly

### Running the Simulation

The simulation is configured in `rv_sim.sv` and includes:

1. **Memory Initialization**:
   - Instruction memory loaded from `imem.hex`
   - Data memory loaded from `dmem_init.hex`

2. **Execution**:
   - Clock period: 10 time units
   - Reset: Active for 5 clock cycles
   - Timeout: 10,000 time units

3. **Completion**:
   - The processor signals completion by writing `0xDEAD` to address `0xFFFF`
   - Final data memory state is written to `dmem_out.hex`

### Using a Simulator

**Example with ModelSim/Questa:**
```bash
# Compile all SystemVerilog files
vlog rv_top.sv rv_dp.sv rv_ctl.sv rv_sim.sv

# Run simulation
vsim -c rv_sim -do "run -all; quit"
```

**Example with VCS:**
```bash
# Compile and simulate
vcs -full64 -sverilog rv_top.sv rv_dp.sv rv_ctl.sv rv_sim.sv
./simv
```

## Memory Organization

### Instruction Memory (IMEM)
- **Size**: 1024 words (4KB)
- **Width**: 32 bits
- **Addressing**: Word-aligned (addresses must be multiples of 4)
- **Initialized from**: `imem.hex`

### Data Memory (DMEM)
- **Size**: 1024 words (4KB)
- **Width**: 32 bits
- **Addressing**: Word-aligned (addresses must be multiples of 4)
- **Initial state**: `dmem_init.hex`
- **Final state**: `dmem_out.hex` (after simulation)

### Special Addresses
- **Completion Signal**: Write `0xDEAD` to address `0xFFFF` to terminate simulation

## How It Works

### Instruction Execution Flow

Each instruction goes through multiple clock cycles:

1. **FETCH**: 
   - PC → Instruction Memory
   - IR ← Instruction Memory[PC]
   - PC ← PC + 4
   - PCC ← PC (save current PC)

2. **DECODE**:
   - Decode opcode and function fields
   - Calculate potential branch target
   - Read source registers

3. **EXECUTE** (varies by instruction type):
   - **Load/Store**: Calculate effective address
   - **ALU Operations**: Perform computation
   - **Branch**: Compare operands and update PC if needed
   - **Jump**: Calculate target and save return address

4. **MEMORY** (for load/store):
   - Access data memory

5. **WRITEBACK**:
   - Write result to destination register

### State Machine Example

For a `LW` (Load Word) instruction:
```
FETCH → DECODE → LSW_ADDR → LW_MEM → LW_WB → FETCH
```

For an `ADD` (R-type) instruction:
```
FETCH → DECODE → RTYPE_ALU → RTYPE_WB → FETCH
```

## Example Program

The included `test.s` demonstrates basic operations:

```assembly
.text
main:
    lw t0, 0x8 (x0)         # Load 0xdead from memory[0x8] into t0
    addi t1, t0, 0xC        # t1 = (0xdead + 0xC) XOR 0xFFFFFFFF
    sw t1, 0x10 (x0)        # Store t1 to memory[0x10]
    
    add t6, x0, x0          # t6 = 0
    beq t6, x0, finish      # Branch to finish
    
deadend: 
    beq t6, x0, deadend     # Infinite loop
    
finish:
    lw t4, 0(x0)            # Load completion address (0xFF00)
    lw t5, 4(x0)            # Load completion value (0xDEAD)
    sw t5, 0xFF(t4)         # Write 0xDEAD to 0xFFFF (signals completion)
    beq t6, x0, deadend     # Infinite loop
```

### Initial Data Memory (`dmem_init.hex`)
```
ff00    # Address 0x00: Completion address pointer
dead    # Address 0x04: Completion value
bed     # Address 0x08: Test data
feed    # Address 0x0C: Test data
```

## Monitoring and Debug

The simulation includes monitoring features:

- **Instruction Trace**: Displays PC and IR when entering DECODE state
- **Completion Detection**: Automatically detects the completion write sequence
- **Timeout Protection**: Prevents infinite simulation runs

Example output:
```
# *** New command start: PC = 00000000, IR = 00802283
# *** New command start: PC = 00000004, IR = 00C28313
# Detected completion write sequence. Exiting.
```

## Design Parameters

Key parameters defined in `params.inc`:

- **DPWIDTH**: 32 bits (datapath width)
- **RFSIZE**: 32 registers
- **IMEM_SIZE**: 1024 words
- **DMEM_SIZE**: 1024 words

## Notes

- The processor implements a **custom ADDI instruction** that performs `(rs1 + imm) XOR 0xFFFFFFFF`
- Register x0 is hardwired to 0 as per RISC-V specification
- All memory accesses must be word-aligned (4-byte aligned)
- The design is synthesizable but primarily intended for simulation and education

## License

This project is part of the Technion EE 044252 course materials.

## Author

Course: Digital Systems and Computer Structure (EE 044252)  
Institution: Technion - Israel Institute of Technology
