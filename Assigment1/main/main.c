/**
 ******************************************************************************
 * @project        : EE2028 Assignment 1 Program Template
 * @file           : main.c
 * @author         : Eda KOKSAL AHMED, ECE, NUS
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
#include "time.h"
#include "stdlib.h"

 /*
 * CONSTANTS
 * ------------------
 * #define Srow 20
 * #define column 2
 * #define Scen 2
 * #define Mcen 3
 * #define Hcen 4
 */
 
 
/*
 * RAND_MAX
 * ------------------
 * Several libraries supposedly define RAND_MAX in <stdlib.h> to conform the ANSI standard.  
 * If RAND_MAX is undefined, this line defines RAND_MAX to be the largest positive integer
 * #define RAND_MAX ((int) ((unsigned) ~0 >> 1))
 */
 
 /*
 * To enable printf() using semihosting
 * ------------------
 */
extern void initialise_monitor_handles(void);


 /*
 * Functions to be written in 
 * ------------------
 */

extern void classification(int* arg1, int* arg2, int* arg3, int* arg4);
extern void find_new_centroids(int* arg1, int* arg2, int* arg3, int* arg4);
extern int elbow(int* arg1, int* arg2, int* arg3);
extern void optimalCluster(int* arg1);

int main(void)
{
	// Necessary function to enable printf() using semihosting
	initialise_monitor_handles();
	
	// Initialize random number generator. 
	rand(time(0)); 
	int i,j;
	int temp1, temp2, temp3, temp4;
	double points [Srow][column]={};
	int points10 [Srow][column]={};
	
	double centroids[Scen][column]; // 2 centroids
	int centroids10[Scen][column];
	
	double Mcen_centroids[Mcen][column]; // 3 centroids
	int Mcen_centroids10[Mcen][column];
	
	double Hcen_centroids[Hcen][column]; // 4 centroids
	int Hcen_centroids10[Hcen][column];
	
	int class[Srow] = {0,0,0,0,0,0,0,0}; // The class of data points when there are 2 centroids
	int Mcen_class[Srow] = {0,0,0,0,0,0,0,0}; // The class of data points when there are 3 centroids
	int Hcen_class[Srow] = {0,0,0,0,0,0,0,0}; // The class of data points when there are 4 centroids
		
	int new_centroids10[Scen][column] = {{Scen,column},{0,0}};
	int new_Mcentroids10[Mcen][column] = {{Mcen,column},{0,0}};
	int new_Hcentroids10[Hcen][column] = {{Hcen,column},{0,0}};
	
	int elbow_wcss[3]={0}; // To store the WCSS value for three different centroids
	
	// Generate random points and Multiply the coordinates by 10 so that the final answers have 1 decimal point
	for(int r=0; r<Srow; r++)//row	
	{ 
	    for(int c=0; c<column; c++)
	    {
	        points [r][c] = ( (double)rand() * ( 4.0 - 0.0 ) ) / (double)RAND_MAX + 0.0;
			points10 [r][c] = Spoints [r][c] * 10;
	    }
    }	
	printf("\n");
	
	// Multiply the coordinates by 10 so that the final answers have 1 decimal point
	for (i=0; i<Scen; i++)
		for (j=0; j<column; j++)
			centroids10[i][j] = centroids[i][j]*10;
	
	/*
	* The code needs to work for 3 different number of clusters, you may choose to write a if-else statement or call the function multiple times. It depends on your coding ! 
	* The code below works only for 2 centroids with small scale data points. You need to update the code so it will work for 3 different centroids for Euclidean distance and Manhattan distance. 
	*/
	int num_centroids=2;
	//for (int num_centroids = 2;num_centroids<5;num_centroids++)
	//{	
		// Binary Classification
		classification((int*)points10, (int*)centroids10, (int*)class, (int*)new_centroids10);
		printf("Class for each point: \n");
		for (i=0; i<Srow; i++)
		{
			printf("point %d: class %d \n", i, class[i]);

		}
		printf("\n");
	
	
	
		// Re-computation of centroids
		find_new_centroids((int*)points10, (int*)centroids10, (int*)class, (int*)new_centroids10);
		printf("New centroids: \n");
		for (i=0; i<Scen; i++)
		{
			temp1 = new_centroids10[i][0] / 10;
			temp2 = new_centroids10[i][0] % 10;
			temp3 = new_centroids10[i][1] / 10;
			temp4 = new_centroids10[i][1] % 10;
			printf("(%d.%d, %d.%d)\n",temp1, temp2, temp3, temp4);

		}
		printf("\n");
		
		// Computation of WCSS to find the optimal number of centroids. 
		elbow_wcss[num_centroids] = elbow((int*)points10,(int*)new_centroids10,(int*)class);
	//}
	
	// Find the optimal number of cluster and display 
	optimalCluster((int*)elbow_wcss);
	
	// Infinite loop
	while(1){}
	
	
}