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


// bigPixel(x,y,c)
bigPixel:
	push {r4-r8,lr}
	mov r4, #10
	mul r4, r0, r4 // x pos
	mov r8, #10
	mul r8, r8, r1 // save y pos

	add r6, r4, #10 // Right corner of big pixel
	add r7, r8, #10 // bottom corner of big pixel	
	
bigPixelXLoop: cmp r4, r6 
	bge returnBigPixel
	mov r5, r8

bigPixelYLoop: cmp r5, r7
	bge doneYLoop
	mov r0, r4
	mov r1, r5
	bl setPixel
	add r5, r5, #1
	b bigPixelYLoop

doneYLoop:
	add r4, r4, #1
	b bigPixelXLoop
	
returnBigPixel: pop {r4-r8,lr}
	bx lr
	
_start:
	mov r0, #7
	mov r1, #5
	ldr r2, =0xd81f
	bl bigPixel
	
_end:
	b .