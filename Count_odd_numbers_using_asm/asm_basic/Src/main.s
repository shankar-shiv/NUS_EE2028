/*
 * Simple ARM assembly file to demonstrate basic asm instructions.
 */

	.syntax unified
	.global main

@ Equates, equivalent to #define in C program
	.equ C,	20 @ #define C 20
	.equ D,	400 @ #define D 400
	.equ START, 50
	.equ COUNTER, 50
	.equ END, 100

main:
@ Code starts here
@ Calculate ANSWER = A*B + C*D

	LDR R0, A
	LDR R1, B
	LDR R7, [R0, #4]! @ pre-offset;
	MUL R0, R0, R1 @ R0 <- R0 * R1
	LDR R1, =C @ Pseudo instruction
	LDR R2, =D @ Pseudo Instruction
	MLA R0, R1, R2, R0
	MOV R4, R0 @ copy and paste the value
	LDR R3, =ANSWER @ Load ANSWER memory address into R3
	STR R4, [R3] @ store r4 value into the address of r3

	MOV R5, #0 @ Clear registers to 0
	MOV R6, #0
	MOV R7, #0
	MOV R8, #0

	LDR R5, =START @ hold number 50
	LDR R6, =2
	LDR R7, =END @ hols number 100

	LDR R8, =COUNTER
	MOV R0, #0 @ hold the divided result
	MOV R1, #0 @ used to compare the remainder
	MOV R2, #0 @


	BL function1 @ jmp to function1

function1:
	CMP R8, #100 @ check if R8 == 100
	IT EQ
	BEQ HALT @if the loop has finished, end

	@ Check if its an odd value
	UDIV R0, R8, R6 @ 50 / 2 = 25, 51 / 2 = 25
	MLS R1, R6, R0, R8 @ R0 = R8 - (R6 * R0); gets the remainder either 1 or 0

	@
	CMP R1, #1 @ if its an odd value by 1-1=0
	ITTTE EQ
	ADDEQ R3, #4 @ Increment the memory address of ADDRESS + 4
	STREQ R8, [R3] @ Store the value at that memory address
	ADDEQ R8, #1 @ Increment the counter
	ADDNE R8, #1 @ Just increment the counter

	B function1

HALT:
	B HALT

@ Define constant values
A:	.word	100
B:	.word	50

@ Store result in SRAM (4 bytes), static random access memory
.lcomm	ANSWER	4
.end
