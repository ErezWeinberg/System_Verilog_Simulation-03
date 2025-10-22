# Documentation Index

This file helps you navigate the documentation for the RISC-V Multicycle Processor Simulation project.

## Quick Start

1. **New to the project?** Start with [README.md](README.md)
   - Project overview and architecture
   - Quick start guide for running simulations
   - File structure and basic usage

2. **Need technical details?** See [DOCUMENTATION.md](DOCUMENTATION.md)
   - Complete state machine details
   - Control signal specifications
   - Timing diagrams
   - Instruction formats
   - Design decisions and rationale

3. **Want to understand the code?** Check the inline comments:
   - [params.inc](params.inc) - All constant definitions with explanations
   - [test.s](test.s) - Example test program with detailed comments
   - SystemVerilog modules have header comments explaining their purpose

## Documentation Files

### README.md
**Target Audience**: Everyone  
**Content**:
- Project overview and objectives
- Architecture description
- Supported instructions
- Simulation instructions
- Example program walkthrough
- Memory organization

### DOCUMENTATION.md
**Target Audience**: Advanced users, developers, students studying computer architecture  
**Content**:
- Detailed datapath component descriptions
- Complete control signal reference
- State machine transitions and timing
- Instruction encoding formats
- ALU operations reference
- Memory interface specifications
- Timing diagrams for each instruction type
- Design decisions and trade-offs
- Extension possibilities

### params.inc
**Target Audience**: Developers modifying the design  
**Content**:
- Instruction opcode definitions
- Control signal constants
- ALU operation encodings
- Immediate format selectors
- Inline comments explaining each parameter

### test.s
**Target Audience**: Users creating new test programs  
**Content**:
- Example RISC-V assembly program
- Demonstrates all supported instructions
- Shows completion sequence for simulation
- Comments explaining each instruction

## File Organization

```
System_Verilog_Simulation-03/
│
├── Documentation/
│   ├── README.md              # Main documentation (you are here)
│   ├── DOCUMENTATION.md       # Technical details
│   └── DOCS_INDEX.md          # This file
│
├── Design Files/
│   ├── rv_top.sv              # Top-level module
│   ├── rv_dp.sv               # Datapath
│   ├── rv_ctl.sv              # Control unit
│   ├── rv_sim.sv              # Testbench
│   └── params.inc             # Parameter definitions
│
├── Test Files/
│   ├── test.s                 # Assembly test program
│   ├── imem.hex               # Instruction memory (compiled from test.s)
│   ├── dmem_init.hex          # Initial data memory values
│   └── dmem_out.hex           # Output data memory (generated after simulation)
│
└── Configuration/
    └── (Simulator-specific files as needed)
```

## Common Tasks and Where to Find Help

### Running Your First Simulation
→ **README.md** - Section: "Simulation"

### Understanding the State Machine
→ **DOCUMENTATION.md** - Section: "State Machine"

### Adding a New Instruction
1. **README.md** - Understand current instruction support
2. **DOCUMENTATION.md** - Study instruction formats and state transitions
3. **params.inc** - Add new opcode definition
4. **rv_ctl.sv** - Add state transitions and control signals
5. **rv_dp.sv** - May need to modify datapath if instruction requires new hardware

### Modifying the Test Program
→ **test.s** - See existing examples and comments

### Understanding Control Signals
→ **DOCUMENTATION.md** - Section: "Control Signals"

### Debugging a Simulation
1. **README.md** - Section: "Monitoring and Debug"
2. **DOCUMENTATION.md** - Section: "Timing Diagrams"
3. Check simulation output for instruction traces

### Understanding Memory Layout
→ **README.md** - Section: "Memory Organization"

## Learning Path

For students learning computer architecture:

1. **Week 1: Basics**
   - Read README.md completely
   - Understand the multicycle concept
   - Run the provided test program

2. **Week 2: Architecture**
   - Study DOCUMENTATION.md sections on:
     - Architecture Details
     - State Machine
     - Control Signals
   - Trace through one instruction execution manually

3. **Week 3: Deep Dive**
   - Study instruction formats
   - Understand timing diagrams
   - Modify test.s to add your own test cases

4. **Week 4: Implementation**
   - Read through all SystemVerilog source files
   - Understand the control signal generation
   - Consider how to add new instructions

## Additional Resources

### RISC-V Resources
- [RISC-V ISA Specification](https://riscv.org/specifications/)
- [RISC-V Assembly Programmer's Manual](https://github.com/riscv-non-isa/riscv-asm-manual)

### Computer Architecture Textbooks
- "Computer Organization and Design: RISC-V Edition" by Patterson & Hennessy
- "Digital Design and Computer Architecture: RISC-V Edition" by Harris & Harris

### SystemVerilog Resources
- IEEE 1800-2017 SystemVerilog Standard
- "SystemVerilog for Verification" by Chris Spear

## Contributing

If you find errors in the documentation or have suggestions for improvement:

1. Check if the issue exists in the latest version
2. Verify your understanding by cross-referencing multiple documentation sources
3. Submit clear, specific feedback with:
   - Location (file and section)
   - Current content (if incorrect)
   - Suggested correction
   - Rationale for the change

## Version History

- **v1.0** (October 2024)
  - Initial comprehensive documentation
  - README.md with quick start guide
  - DOCUMENTATION.md with technical details
  - Inline code comments in params.inc and test.s
  - Documentation index (this file)

## Contact and Support

For questions related to:
- **Course Material**: Contact Technion EE 044252 course staff
- **Technical Issues**: Refer to DOCUMENTATION.md troubleshooting sections
- **RISC-V Specification**: See RISC-V Foundation resources

---

*This documentation is part of the Technion EE 044252: Digital Systems and Computer Structure course.*
