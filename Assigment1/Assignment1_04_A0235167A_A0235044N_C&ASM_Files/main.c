/**
 ******************************************************************************
 * @project        : EE2028 Assignment 1 Program Template
 * @file           : main.c
 * @author         : Shankar (A0235167A), Wei Heng (A0235044N)
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

// #define M 6	 // No. of numbers in array
// #define M 3	 // No. of numbers in array

// Necessary function to enable printf() using semihosting
extern void initialise_monitor_handles(void);

// Function Definitions
extern int bubble_sort(int *arg1, int arg2);
int BubbleSort(int arr[], int size);

int main(void) {
	// Necessary function to enable printf() using semihosting
	initialise_monitor_handles();

	int arr[] = { -18, -34, -32, -75, 11, 97 };
	int arr_for_c[] = { -18, -34, -32, -75, 11, 97 };
	int swap, i;  // no. of total swaps

	int M = sizeof(arr) / sizeof(int);
	printf("Length of the array = %d \n", M);

	// Bubble sort with bubble_sort.s
	swap = bubble_sort((int*)arr, (int)M);
	printf("After %d rounds of swap, the array is sorted as: \n{ ", swap);

	for (i = 0; i < M; i++) {
		printf("%d ", arr[i]);
	}
	printf("}\n");

	// Bubble sort implemented in C
	swap = BubbleSort(arr_for_c, (int) M);
	printf("\n It took %d rounds of swap for the array to be sorted. \n{ ", swap);

	// Infinite loop
	while (1) {
	}

}

int BubbleSort(int arr[], int size) {
	int i, j, temp, flag = 0;
	int total_swap_counter = 0;

	for (i = 0; i < size; i++) {
		flag = 0;

		for (j = 0; j < size - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				flag = 1;
				temp = arr[j + 1]; // Swap operation
				arr[j + 1] = arr[j];
				arr[j] = temp;
				total_swap_counter += 1;
			}
		}
		if (flag == 0) { // array is already sorted
			// printf("\n Array is already Sorted!!");
			printf("\n The array Sorted in ascending order is :\n");
			printf("{");
			for (i = 0; i < size; i++) {
				printf(" %d ", arr[i]);
			}
			printf("}");
			return total_swap_counter;
		}
	}
	return total_swap_counter;
}
