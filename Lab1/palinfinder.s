.section .text
.global _start


// Function checks if input is an exception
isNotException:
    cmp r0, #'?'
    beq exception
    cmp r0, #'#'
    beq exception
    mov r0, #1 // is not exception
    b return1
exception: mov r0, #0 // is exception
return1: bx lr


// Function check if input char is a space
isSpace:
    cmp r0, #' '
    beq space
    mov r0, #0 // is not space 
    b return2
space: mov r0, #1 // is space 
return2: bx lr


// Prints statement and turn on leds for a palindrome
outputIsPalindrome:
    //Print in JTAG UART
    ldr r0, =isPalindromeStr
    mov r1, #20 // length of string
    push {lr}
    bl printString
    pop {lr}
	
    //Turn on leds
    ldr r1, =0xff200000 // LEDs base address
    mov r2, #0b0000011111
    str r2, [r1]
	
return3: bx lr


// Prints statement and turn on leds for a non-palindrome
outputIsNotPalindrome:
    //Print in JTAG UART
    ldr r0, =noPalindromeStr
    mov r1, #17 // length of string
    push {lr}
    bl printString
    pop {lr}

    //Turn on leds
    ldr r1, =0xff200000 // LEDs base address
    mov r2, #0b1111100000
    str r2, [r1]
return4: bx lr


// Funtion uses JTAG UART to print input string
// input: string address, length
printString:
    add r2, r0, r1 // address of end of string
loop:
    cmp r0, r2 
    beq return4 // if end of string
    ldrb r3, [r0], #1 // character & incrementing r0
    push {r0, r2, lr}
    mov r0, r3 // put character as input
    bl PUT_JTAG // prints character
    pop {r0, r2, lr}
    b loop
return5: bx lr


PUT_JTAG:
    ldr r1, =0xff201000 // JTAG UART base address
    ldr r2, [r1, #4] // read the JTAG UART control register
    ldr r3, =0xffff0000
    ands r2, r2, r3 // check for write space
    beq PUT_JTAG // if no space try agian
    str r0, [r1] // print the character
    bx lr


// Finds length of string
// Input: string base address and length variable address
findStrLength:
    mov r2, #0 // length variable
lengthLoop:
    ldrb r3, [r0], #1 // char i & incrementing r0 with 1
    cmp r3, #0 // check for end of string
    beq storeLength
    add r2, r2, #1 // length+=1
    b lengthLoop
storeLength: str r2, [r1] // store input length
return7: bx lr


// Input: base address of string and length of string
makeAllUpperCase:
	add r3, r0, r1
upperCaseLoop: cmp r0, r3
    beq return8
    ldrb r2, [r0] // letter [i]
    cmp r2, #90
    subgt r2, #32 // make upper case
    strb r2, [r0], #1 // store upper case letter and incremet r0
    b upperCaseLoop 
return8: bx lr


// Input: base address of input
findPalindrome: 
    push {r4, r5, r6, r7, r8} // storing Callee saved values before using registers 
    ldr r0, =input
    ldr r1, =inputLength 
    push {lr, r1, r0}
    bl findStrLength
    pop {lr, r1, r0}
    ldr r1, [r1] // input length
    push {lr}
    bl makeAllUpperCase
    pop {lr}

    mov r6, #0 // i
    sub r7, r1, #1 // j = length-1
    asr r3, r1, #1 // length / 2

palindromeLoop: cmp r6, r3 // check if end of string
    bge return9 // branch if i >= length/2
    cmp r7, r3
    ble return9 // branch if j <= length/2
    
    ldr r0, =input
    ldrb r4, [r0, r6] // input[i]
    ldrb r5, [r0, r7] // input[j]

    // check if only input[i] is a space 
    push {lr}
    mov r0, r4 
    bl isSpace
    pop {lr}
    cmp r0, #1
    bne continue1
    mov r0, r5
    push {lr}
    bl isSpace
    pop {lr}
    cmp r0, #0
    bne continue1
    add r6, r6, #1 // i += 1

    ldr r0, =input
    ldrb r4, [r0, r6] // update input[i]
    b continue
continue1: 
    // check if only input[j] is a space 
    mov r0, r5 
    push {lr}
    bl isSpace
    pop {lr}
    cmp r0, #1
    bne continue
    mov r0, r4
    push {lr}
    bl isSpace
    pop {lr}
    cmp r0, #0
    bne continue
    ldr r0, =input
    sub r7, r7, #1 // j -= 1
    ldrb r5, [r0, r7] // update input[j]

continue: cmp r4, r5
    beq updateCounters
	
    // check if input[i] or input[j] is an exception
    mov r0, r4
    push {lr}
    bl isNotException
    pop {lr}
    cmp r0, #0
    beq updateCounters
    mov r0, r5
    push {lr}
    bl isNotException
    pop {lr}
    cmp r0, #0
    beq updateCounters

    // input is not a palindrome
    ldr r8, =isPalindrome
    mov r0, #0
    str r0, [r8] // isPalindrome = 0
    b return9
updateCounters:
    add r6, r6, #1 // i += 1
    sub r7, r7, #1 // j -= 1
    b palindromeLoop
return9: pop {r4, r5, r6, r7, r8} // laodiing callee saved values before returning from function
    bx lr

_start:
    bl findPalindrome
	
	// get output result
    ldr r1, =isPalindrome
    ldr r1, [r1] // isPalindrome variable
    cmp r1, #1
    beq palindrome
    b notPalindrome
palindrome: bl outputIsPalindrome
    b _end
notPalindrome: bl outputIsNotPalindrome
    
_end:  
  b .


.section .data
.align 
input: .asciz "Was it a car or a cat I saw"
.align 
noPalindromeStr: .asciz "Not a palindrome\n"
.align
isPalindromeStr: .asciz "Palindrome detected\n"

.align 2
isPalindrome: .int 1
.align 2
inputLength: .int 0 

.end 