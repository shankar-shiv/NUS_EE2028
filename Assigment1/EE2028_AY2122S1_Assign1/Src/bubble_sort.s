/*
 * bubble_sort.s
 *
 *  Created on: 4/8/2022
 *      Author: Shankar
 * 		Author: Wei Heng
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
@	R5 temporary register used to swap elements
@	R6 temporary register used to swap elements
@	R7 temporary register used to swap elements
@	R8 Total Swap Counter; increment by 1 if a swap operation is perfomed
@ 	R9 Local Swap Counter; if (R9 == no.of elements) {array is sorted so exit the asm function} else {set to 0}


@ write your program from here:
bubble_sort:

	PUSH {R2-R9} @ Help us preserve the "marking" when we navigate back to main function in main.c

	@Initialise the registers here -- Only Execute for the first time
	LDR R2, =#0 @ i value; outer loop
	LDR R3, =#0 @ j value; inner loop
	SUB R4, R1, #1 @ R4 = N(Number of elements)-1
	LDR R5, =#0 @ temporary register used to swap elements
	LDR R6, =#0 @ temporary register used to swap elements
	LDR R7, =#0 @ temporary register used to swap elements
	LDR R8, =#0 @ Total Swap Counter; increment by 1 if a swap operation is perfomed
	LDR R9, =#0 @ ++1st Additions(4)++ @ Zero Swap Counter; if (R9 == no.of elements) {array is sorted so exit the asm function}

	PUSH {R14} @ Store the return address value to Stack Register safely used to navigate back to main function in main.c

OUTER_LOOP:

	CMP R2, R4 @ check if outer loop counter == 5; if its equal, we have gone through the max no. of iterations
	ITTTT EQ @ Set the equal flag when R2 - R4 = 0
	MOVEQ R0, R8
	POPEQ {R14}
	POPEQ {R2-R9} @ Set the R2-R9 registers back to the original values
	BXEQ LR @if the loop has finished, end the loop to return back to main function in main.c file

	CMP R9, R4 @ ++2nd Additions(4)++ @ check if zero swap counter == 5; if its equal meaning no swap took place, the array is sorted.
	ITTTT EQ @ Set the equal flag when R9 - R4 = 0
	MOVEQ R0, R8
	POPEQ {R14}
	POPEQ {R2-R9} @ Set the R2-R9 registers back to the original values
	BXEQ LR @if the loop has finished, end the loop to return back to main function in main.c file


	@ Reset the variables after every ith iteration (every outer loop iteration)
	LDR R3, =#0 @ j value; inner loop
	LDR R5, =#0 @ temporary register used to swap elements
	LDR R6, =#0 @ temporary register used to swap elements
	LDR R7, =#0 @ temporary register used to swap elements
	LDR R9, =#0 @ ++3rd Additions (4)++ @ set zero swap counter to 0

	PUSH {R0}
	BL INNER_LOOP
	@ <After INNER_LOOP ends, come here>
	POP {R0} @ Reset the R0 pointer back to the memory address pointing to the first element in the array
	ADD R2, #1 @ Increment the outer loop counter

	B OUTER_LOOP @ Loop back to the start of OUTER_LOOP subroutine

INNER_LOOP:
	CMP R3, R4 @ check if inner_loop_counter == 5; if its equal, we have completed 1 iteration
	IT EQ @ Set the equal flag when R2 - R4 = 0
	BXEQ LR @ if the loop has finished, end the loop.

	LDR R5, [R0, #0] @ De-Reference the memory address in (R0 + 0) ... (R0 + 4)
	LDR R6, [R0, #4] @ De-Reference the memory address in (R0 + 4) ... (R0 + 8)

	CMP R6, R5 @ CMP performs R6 - R5
	ITTTT MI @ if the result of the above operation is negative, perform the swap operation
	MOVMI R7, R5 @ Swap the bigger value to temp R7
	MOVMI R5, R6 @ Swap the smaller value to R5
	MOVMI R6, R7 @ Swap the value in temp R7 R6
	STRMI R5, [R0]
	@ Extension of the above code; when the result is negative
	ITTT MI @ if the result is negative, perform the swap operation
	ADDMI R0, 4 @ Move R0 pointer by 4 bytes to point to the next subsequent memory location
	STRMI R6, [R0]
	ADDMI R8, #1 @ Increment the total_swap counter by 1, if swap is done

	ITT PL @ If dont need to swap, come here
	ADDPL R0, 4 @ Move R0 pointer by 4 bytes to point to the next subsequent memory location
	ADDPL R9, #1 @ ++4th Additions(4)++ @ Increment the zero swap counter

	ADD R3, #1 @ Increment the inner loop counter

	B INNER_LOOP @ Loop back to the start of INNER_LOOP subroutine

.end
