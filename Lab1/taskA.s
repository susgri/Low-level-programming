.global _start
.arch armv7ve
 
_start: 
	ldr r0, =0xff201000 @ JTAG_UART data register 
	mov r1, #0 @ counter
	mov r12, #0x2c @ ASCII for comma
	mov r11, #0x20  @ASCII for space
	

_loop:
	mov r2, r1  
	mov r3, #10
	sdiv r4, r2, r3 @ ten digit 
	mul r3, r3, r4 
	sub r2, r2, r3 @ ones digit
	
	@ converting to ASCII and printing
	cmp r4, #0
	beq _skipTens
	add r4, r4, #'0'
	str r4, [r0]
	
_skipTens:
	add r2, r2, #'0'
	str r2, [r0]
	@ printing commma and space 
	str r12, [r0]
	str r11, [r0]
	
	@ checks loop condition and increments counter
	cmp r1, #99
	add r1, r1, #1
	bne _loop
	
_exit:
	@ reset all registers 
	mov r0, #0
	mov r1, #0
	mov r2, #0
	mov r3, #0
	mov r4, #0
	mov r11, #0
	mov r12, #0
	
	b .
		
.data
	JTAG_UART_BASE: .word 0xFF201000