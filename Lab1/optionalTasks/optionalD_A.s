.global _start

setPixel: // assumes R0 = x-coordinate, R1 = y-coord, R2 = color value
	push {r4-r6,lr}
    lsl r4, r1, #10 // y-address shift left 10 bits (1024 bytes)
	lsl r5, r0, #1  // x-address  shift left 1 bit (2 bytes)
    add r4, r5 // R4 is now correct offset
    ldr r6, =0xc8000000	
    strh r2, [r6,r4]  // Store half-word, lower 16 bits at address R6 + offset R4
    pop {r4-r6,lr}
	bx lr
	
_start:
	mov r4, #0 // x-coordinate
	mov r5, #0 // y-coordinate
	mov r0, r4
	mov r1, r5 
	ldr r2, =0xd81f // rosa
	bl setPixel
	add r4, r4, #1
	mov r0, r4
	ldr r2, =0xfbe0 // oransje
	bl setPixel
	add r5, r5, #1
	mov r1, r5
	ldr r2, =0x07e0 // oransje
	bl setPixel
	sub r4, r4, #1
	mov r0, r4
	ldr r2, =0xffe0 // oransje
	bl setPixel
	
_end:
	b .