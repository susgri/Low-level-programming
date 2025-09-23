.global _start
_start:
	ldr r12, =0xff201000 @ JTAG_UART data register 
	ldr r0, =input @ address of input
	mov r1, #0 @ counter
	
_findLenth:
	add r2, r0, r1 @ current character address
	ldrb r3, [r2] @ store the character in r3
	
	cmp r3, #0x0 @ check for null byte at end of string
	beq _print
	
	add r1, r1, #1 @ incrementing counter
	b _findLenth

_print:
	add r2, r0, r1 @ current character address
	ldrb r3, [r2] @ store the character in r3
	
	strb r3, [r12]
	
	cmp r1, #0
	beq _exit 
	sub r1, r1, #1 @ decrementing counter
	b _print

_exit:
	mov r0, #0
	mov r1, #0
	mov r2, #0
	mov r3, #0
	mov r12, #0
	b .

.data
.align
	input: .asciz "Hello world"
	
	