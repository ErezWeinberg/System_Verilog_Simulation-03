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