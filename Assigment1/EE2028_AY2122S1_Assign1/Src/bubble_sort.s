/*
 * bubble_sort.s
 *
 *  Created on: 4/8/2022
 *      Author: Shankar
 */
   .syntax unified
	.cpu cortex-m4
	.fpu softvfp
	.thumb

		.global bubble_sort

@ Start of executable code
.section .text

@ EE2028 Assignment 1, Sem 1, AY 2022/23
@ (c) ECE NUS, 2022
@ Bubble sort arr in decending order

@ Write Student 1’s Name here: Shankar
@ Write Student 2’s Name here: Wei Heng

@ You could create a look-up table of registers here:

@ 	R0 contains the address of the first element in the array
@ 	R1 contains the literal value 6
@	R2 i; outer loop counter
@	R3 j; inner loop counter
@	R4 len(array) - 1
@	R5 temporary register
@	R6 temporary register
@	R7 temporary register
@	R8 Global Swap Counter


@ write your program from here:
bubble_sort:

	PUSH {R2-R8}

	@Initialise the registers here
	LDR R2, =#0 @ i value; outer loop
	LDR R3, =#0 @ j value; inner loop
	SUB R4, R1, #1 @ R4 = N(Number of elements)-1
	LDR R5, =#0 @ temporary register
	LDR R6, =#0 @ temporary register
	LDR R7, =#0 @ temporary register
	LDR R8, =#0 @ Swap Counter

	PUSH {R14}
	@ START
	@B SUBROUTINE -- Backup Plan
FUNCTION1:

	@ Reset the variables
	LDR R3, =#0 @ j value; inner loop
	LDR R5, =#0 @ temporary register
	LDR R6, =#0 @ temporary register
	LDR R7, =#0 @ temporary register

	@PUSH {R14} @ Store the return address value to Stack register safely!

	CMP R2, R4 @ check if R2 == 5; if its equal ...
	ITTTT EQ @ Set the equal flag when R2 - R4 = 0
	MOVEQ R0, R8
	POPEQ {R14}
	POPEQ {R2-R8}
	BXEQ LR @if the loop has finished, end the loop.

	PUSH {R0}

	BL LOOP
	@ <After LOOP ends, come here>
	POP {R0}

	ADD R2, #1 @ Increment the outer loop counter

	@POP {R14}

	B FUNCTION1
	@ END

	@ Should not execute HERE!!!
	@ BX LR

@ Backup Plan
SUBROUTINE:
	CMP R2, R4 @ check if R2 == 5; if its equal ...
	IT EQ @ Set the equal flag when R2 - R4 = 0
	BXEQ LR @if the loop has finished, end the loop.

	PUSH {R0}

	B LOOP
	@ <come here>
	POP {R0}

	ADD R2, #1 @ Increment the outer loop counter

	B SUBROUTINE @ Loop SUBROUTINE
	@ BX LR
LOOP:
	CMP R3, R4 @ check if R3 == 5; if its equal ...
	IT EQ @ Set the equal flag when R2 - R4 = 0
	BXEQ LR @if the loop has finished, end the loop.

	LDR R5, [R0, #0] @ 34 --> 32
	LDR R6, [R0, #4] @ 32 --> 34

	CMP R6, R5 @ R6 - R5
	ITTTT MI @ if the result is negative, conduct the swap operation
	MOVMI R7, R5 @ Swap the bigger value to temp R7
	MOVMI R5, R6 @ Swap the smaller value to R5
	MOVMI R6, R7 @ Swap the value in temp R7 R6
	STRMI R5, [R0]
	@ Extension of the above code; when the result is negative
	ITTT MI @ if the result is negative, conduct the swap operation
	ADDMI R0, 4 @ Postman Walking to next address
	STRMI R6, [R0]
	AddMI R8, #1 @ Add the total_swap counter, if swap is done

	IT PL @ If dont need to swap, come here
	ADDPL R0, 4

	ADD R3, #1 @ Increment the inner loop counter

	B LOOP


@ Store result in SRAM (4 bytes), static random access memory
@.lcomm	ADDRESS	4
.end
