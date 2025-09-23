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
	
blankScreen:
	push {r4-r6,lr}
	ldr r2, =0x000ffff
	mov r4, #0 // x-coordinate
	
xLoop: cmp r4, #320 // 320 pixels in x-dir
	bge returnBlank
	mov r5, #0 // y-coordinate
	
yLoop: cmp r5, #240 // 240 pixels in y-dir
	bge doneYLoop
	mov r0, r4
	mov r1, r5
	bl setPixel
	add r5, r5, #1 // increment y
	b yLoop
	
doneYLoop:
	add r4, r4, #1 // increment x
	b xLoop
	
returnBlank: pop {r4-r6,lr}
	bx lr
	
_start:
	bl blankScreen
	
	
_end:
	b .