
**Hint: Use and re-use the registers in a systematic way. Maintain a data dictionary or table to help you keep track of the storage of different terms in different registers at different times.**



## Question 1: how to access the elements in array from the asm_fun? Given the N-th element's address is X, what would be the address of the (N+k)-th element's address?
- De-Reference the pointer to access the value

## Question 2: Describe what happens with and without PUSH and POP {R14}, explain why is there a difference.
- When entering into subroutine, the registers are modified, so need to manually PUSH and POPthem at the end of the subroutine to restore their original contents. As such, the R14 is modified when the progrsam has entered the subroutine, its contents (Link Register) are modified, unable to go back to main program. 
- If the content of this register is lost, the program will not be able to return correctly from the subroutine to the calling part of the program.

## Question 3: What can you do if you have used up all the general purpose registers and you need to store some more values during processing?
- Use and re-use the registers in a systematic way. Maintain a data dictionary or table to help you keep track of the storage of different terms in different registers at different times.

