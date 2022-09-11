/*
 * bubble_sort.s
 *
 *  Created on: 4/8/2022
 *      Author: Shankar (A0235167A)
 * 		Author: Wei Heng (A0235044N)
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
@ Bubble sort array in ascending order

@ Write Student 1’s Name here: Shankar
@ Write Student 2’s Name here: Wei Heng

@ You could create a look-up table of registers here:

@ 	R0 contains the address of the first element in the array
@ 	R1 contains the literal value 6

@	R2 i; outer loop counter and j; inner loop counter 
@	R3 number of elements - 1
@	R4 temporary register used to swap elements
@	R5 temporary register used to swap elements
@	R6 Total Swap Counter; increment by 1 if a swap operation is perfomed
@ 	R7 Local Swap Counter; if (R9 == no.of elements) {array is sorted so exit the asm function} else {set to 0}

bubble_sort:

	PUSH {R2-R7} @ Help us preserve the "marking" when we navigate back to main function in main.c
	PUSH {R14} @ Store the return address value to Stack Register safely used to navigate back to main function in main.c

	@ Initialize the registers here -- Execute this portion for the first time ONLY
	LDR R2, =#0 @ Set Loops counter to 0
	SUB R3, R1, #1 @ R3 = N(Number of elements)-1
	LDR R4, =#0 @ temporary register used to swap elements
	LDR R5, =#0 @ temporary register used to swap elements
	LDR R6, =#0 @ Total Swap Counter; increment by 1 if a swap operation is perfomed
	LDR R7, =#0 @ Zero Swap Counter

	B OUTER_LOOP @ Branch to OUTER_LOOP

OUTER_LOOP:
	CMP R2, R3 @ check if outer loop counter == 5; if its equal, we have gone through the max no. of iterations
	ITTTT EQ @ Set the equal flag when R2 - R3 = 0
	MOVEQ R0, R6 @ Move R6 total swap counter value to R0 to return back to the main function in main.c
	POPEQ {R14}
	POPEQ {R2-R7} @ Set the R2-R7 registers back to the original values
	BXEQ LR @if the loop has finished, end the loop to return back to main function in main.c file

	CMP R7, R3 @ Check if zero swap counter == 5; if its equal, meaning the array is already sorted.
	ITTTT EQ @ Set the equal flag when R7 - R3 = 0
	MOVEQ R0, R6 @ Move R6 total swap counter value to R0 to return back to the main function in main.c
	POPEQ {R14}
	POPEQ {R2-R7} @ Set the R2-R7 registers back to the original values
	BXEQ LR @if the loop has finished, end the loop to return back to main function in main.c file

	PUSH {R2} @ Retaining OUTER_LOOP counter
	PUSH {R0} @ Retaining array 1st address

	@ Reset the variables after every ith iteration (every outer loop iteration)
	LDR R2, =#0 @ initialise counter for INNER_LOOP usage
	LDR R4, =#0 @ temporary register used to store elements; not necessary to perform this instruction
	LDR R5, =#0 @ temporary register used to store elements; not necessary to perform this instruction
	LDR R7, =#0 @ set zero swap counter to 0

	BL INNER_LOOP

	@ After INNER_LOOP completed, execute the following:
	POP {R0} @ Reset the R0 pointer back to the memory address pointing to the first element in the array
	POP {R2} @ Retrieving previous OUTER_LOOP counter
	ADD R2, #1 @ OUTER_LOOP counter increment

	B OUTER_LOOP @ Loop back to the start of OUTER_LOOP subroutine

INNER_LOOP:
	CMP R2, R3 @ check if inner_loop_counter == 5; if its equal, we have completed 1 iteration
	IT EQ @ Executes when R2 - R3 = 0
	BXEQ LR @ If the loop has finished, links back to OUTER_LOOP.

	LDR R4, [R0, #0] @ De-Reference the memory address in (R0 + 0) ... (R0 + 4)
	LDR R5, [R0, #4] @ De-Reference the memory address in (R0 + 4) ... (R0 + 8)

	CMP R5, R4 @ CMP performs R5 - R4
	ITTTT MI @ If the result of the above operation is negative, perform the swap operation
	STRMI R5, [R0] @ Storing smaller value to address R0
	ADDMI R0, #4 @ Increment R0 pointer by 4
	STRMI R4, [R0] @ Storing larger values to next address
	ADDMI R6, #1 @ Add swap count

	ITT PL @ If dont need to swap, come here
	ADDPL R0, #4 @ Move R0 pointer by 4 bytes to point to the next subsequent memory location
	ADDPL R7, #1 @ Increments the zero swap counter by 1

	ADD R2, #1 @ Increment the INNER_LOOP counter

	B INNER_LOOP @ Loop back to INNER_LOOP subroutine

.end
