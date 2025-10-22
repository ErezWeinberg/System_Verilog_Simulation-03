 // Design parameters
 // =================
 // This file contains all the constant definitions used across the processor design.
 // These parameters define instruction opcodes, control signals, and ALU operations.

 // Instruction Opcodes (opcode[6:0] + funct3[2:0])
 // ================================================
 // The processor decodes instructions by examining the opcode and funct3 fields.
 // 'x' represents don't-care bits in the matching logic.
 localparam
    LW      = 10'b0000011_010,  // Load Word: opcode=0000011, funct3=010
    SW      = 10'b0100011_010,  // Store Word: opcode=0100011, funct3=010
    ALU     = 10'b0110011_xxx,  // R-type ALU operations: opcode=0110011
    BEQ     = 10'b1100011_000,  // Branch Equal: opcode=1100011, funct3=000
    JAL     = 10'b1101111_xxx,  // Jump and Link: opcode=1101111
    ADDI    = 10'b0010011_000;  // Add Immediate: opcode=0010011, funct3=000 (custom XOR implementation)

 // PC Source Selector
 // ==================
 // Controls the source of the next PC value
 localparam
    PC_ALU = 1'b1,  // Next PC comes from ALU output (for branches/jumps)
    PC_INC = 1'b0;  // Next PC is PC+4 (sequential execution)

 // Immediate Format Selector
 // ==========================
 // Selects which immediate format to use based on instruction type
 localparam
    IMM_B   = 2'b00,  // B-format: Branch instructions (12-bit signed, shifted)
    IMM_J   = 2'b01,  // J-format: Jump instructions (20-bit signed, shifted)
    IMM_S   = 2'b10,  // S-format: Store instructions (12-bit signed)
    IMM_L   = 2'b11;  // I/L-format: Load and immediate ALU instructions (12-bit signed)

 // ALU Input A Selector
 // ====================
 // Selects the source for ALU input A
 localparam
    ALUA_PCC = 2'b00,  // PC Copy register (used for branch target calculation)
    ALUA_REG = 2'b01,  // Register file output A
    ALU_OUT  = 2'b10;  // ALU output register (feedback for multi-cycle operations)

 // ALU Input B Selector
 // ====================
 // Selects the source for ALU input B
 localparam
    ALUB_IMM = 2'b00,  // Immediate value from instruction
    ALUB_REG = 2'b01,  // Register file output B
    FFFFFFF  = 2'b10;  // Constant 0xFFFFFFFF (used for custom ADDI XOR operation)

 // ALU Operation Selector
 // ======================
 // Format: {funct3[2:0], modifier}
 // The modifier bit differentiates between similar operations (e.g., SRL vs SRA)
 localparam
    ALU_ADD  = 4'b000_0,  // Addition: a + b
    ALU_SUB  = 4'b000_1,  // Subtraction: a - b
    ALU_SLL  = 4'b001_0,  // Shift Left Logical: a << b
    ALU_SLT  = 4'b010_0,  // Set Less Than (signed): a < b ? 1 : 0
    ALU_SLTU = 4'b011_0,  // Set Less Than Unsigned: a < b ? 1 : 0
    ALU_XOR  = 4'b100_0,  // Bitwise XOR: a ^ b
    ALU_SRL  = 4'b101_0,  // Shift Right Logical: a >> b
    ALU_SRA  = 4'b101_1,  // Shift Right Arithmetic: a >>> b (sign-extend)
    ALU_OR   = 4'b110_0,  // Bitwise OR: a | b
    ALU_AND  = 4'b111_0;  // Bitwise AND: a & b

 // Writeback Source Selector
 // ==========================
 // Selects which value to write back to the register file
 localparam
    WB_MDR    = 2'b00,  // Memory Data Register (for load instructions)
    WB_ALUOUT = 2'b01,  // ALU output register (for ALU instructions)
    WB_PC     = 2'b10;  // Program Counter (for JAL instruction - save return address)

