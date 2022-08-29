from random import randint

n = 10
lst = [randint(0, 100) for _ in range(n)]


def bubble(lst):
    # 1. do we want to mutate lst or we want to create a new copy

    # make a copy
    lst_copy = list(lst)  # O(n) time

    for i in range(1, len(lst_copy)):  # O(n) time
        # if left element > right element:
        if lst_copy[i-1] > lst_copy[i]:
            # swap
            # approach 1: standard method to swap values between 2 variables
            ##            temp = lst_copy[i-1]
            ##            lst_copy[i-1] = lst_copy[i]
            ##            lst_copy[i] = temp

            # approach 2 (python only)
            lst_copy[i-1], lst_copy[i] = lst_copy[i], lst_copy[i-1]
    return lst_copy


def bubbleSort(lst):
    for _ in range(len(lst)-1):  # n-1 times
        lst = bubble(lst)  # O(n), return to make a new updated list
    return lst


print(bubbleSort(lst))
