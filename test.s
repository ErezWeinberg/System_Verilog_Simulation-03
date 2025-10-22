// **********************************************************************
// Test Program for RISC-V Multicycle Processor
// **********************************************************************
// This program demonstrates the supported instructions and tests the
// custom ADDI implementation with XOR operation.
//
// Test Sequence:
// 1. Load test value from memory (0xdead from address 0x8)
// 2. Perform custom ADDI: (value + 0xC) XOR 0xFFFFFFFF
// 3. Store result to memory (address 0x10)
// 4. Test branch and jump operations
// 5. Signal completion to simulator
//
// Initial Memory Values (dmem_init.hex):
//   Address 0x00: 0xFF00 (completion address pointer)
//   Address 0x04: 0xDEAD (completion signal value)
//   Address 0x08: 0xBED  (test data)
//   Address 0x0C: 0xFEED (test data)
// **********************************************************************

.text
main:   # Put your code here
		lw t0, 0x8 (x0) 		#t0 = 0xdead
		addi t1 , t0 , 0xC		#t1 = (0xdead + 0xC)^(0xfffffff)
		sw t1 ,0x10 (x0)		#set the mem 0x10 with the value int1
		
        add		t6, x0, x0
        beq		t6, x0, finish

		
deadend: beq	t6, x0, deadend        

finish:
        lw		t4, 0(x0)
        lw		t5, 4(x0)
        sw		t5, 0xFF(t4)
        beq		t6, x0, deadend