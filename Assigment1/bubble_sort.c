// program to enter n numbers in an array.
// Redisplay the array with elements being sorted in ascending order.

// Bubble Sort method (With improved Time Complexity).

#include <stdio.h>
#include <stdlib.h>

int BubbleSort(int arr[], int size)
{
    int i, j, temp, flag = 0;
    int total_swap_counter = 0;

    for (i = 0; i < size; i++)
    {
        flag = 0; // Set flag to 0
        for (j = 0; j < size - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                flag = 1;
                temp = arr[j + 1];       // Perform Swap operation
                arr[j + 1] = arr[j];     // Perform Swap operation
                arr[j] = temp;           // Perform Swap operation
                total_swap_counter += 1; // Increment the total_swap_counter
            }
        }
        if (flag == 0)
        { // array is already sorted
            // printf("\n Array is already Sorted!!");
            printf("\n The array Sorted in ascending order is :\n");
            for (i = 0; i < size; i++)
            {
                printf(" %d ", arr[i]);
            }
            return total_swap_counter;
        }
    }
    return total_swap_counter;
}

// main function
int main()
{
    int i, size, swap;

    int arr[11] = {-18, 23, 45, -11, -97, -81, 56, 89, 10, 11};
    size = sizeof(arr) / sizeof(int);
    printf("Size of array is %d", size);

    // function bubble sorting(with improved Time Complexity)
    swap = BubbleSort(arr, size);
    printf("\n Total Swaps done: %d \n", swap);

    return 0;
}