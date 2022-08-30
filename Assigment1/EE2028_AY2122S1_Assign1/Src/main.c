/**
 ******************************************************************************
 * @project        : EE2028 Assignment 1 Program Template
 * @file           : main.c
 * @author         : Shankar, Wei Heng
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "stdio.h"

#define M 6	 // No. of numbers in array
// #define M 2	 // No. of numbers in array

// Necessary function to enable printf() using semihosting
extern void initialise_monitor_handles(void);

// Functions to be written
extern int bubble_sort(int* arg1, int arg2);

int main(void)
{
	// Necessary function to enable printf() using semihosting
	initialise_monitor_handles();

	int arr[M] = {18, 34, 32, 75, 11, 97};
	// int arr[M] = {18, 9};
	int swap,i;  // no. of total swaps

	// Bubble sort with bubble_sort.s
	swap = bubble_sort((int*)arr, (int)M);
	printf("After %d rounds of swap, the array is sorted as: \n{ ", swap);

	// int *p = 0x20017FE4;
	// printf("%d \n", *p);

	for (i=0; i<M; i++)
	{
		printf("%d ", arr[i]);
	}
	printf("}\n");

	// Infinite loop
	while(1){}

}
