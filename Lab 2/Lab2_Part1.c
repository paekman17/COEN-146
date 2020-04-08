/*
Name: Ethan Paek
Date: 4/7/20
Title: Lab #2 Part 1– Multithreading in C
Description: Exploit parallelism in large scale multidimensional matrix multiplication. 
*/
/*COEN 146L : Lab1, part 1 SingleThreaded */
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <pthread.h>

#define N 1024
#define M 1024
#define L 1024

pthread_t tids[N];

double matrixA[N][M], matrixB[M][L], matrixC[N][L] = {0.0};

void printMatrix(int nr, int nc, double matrix[nr][nc]);

void *start_routine(void *arg){
    int j, k = 0;
    int i = *(int *)arg;
    for(j = 0; j < L; j++)
        for ( k = 0; k < M; k++)
            matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
    free(arg);
}

int main() {
	srand(time(NULL));
	int i, j, k = 0;

	// generate matrix A
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			matrixA[i][j] = rand(); // rand()
		}
	}

	// generate matrix B
	for (i = 0; i < M; i++) {
		for (j = 0; j < L; j++) {
			matrixB[i][j] = rand(); // rand()
		}
	}
	
	// calculate matrix C by computing A x B
	for (i = 0; i < N; i++){
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&tids[i], NULL, start_routine, arg);
    }
	
	printf("\n\nMatrixA:\n");
	printMatrix(N, M, matrixA);
	printf("\n\nMatrixB:\n");
	printMatrix(M, L, matrixB);
	printf("\n\nMatrixC:\n");
	printMatrix(N, L, matrixC); 
	
  return 0;
}

void printMatrix(int nr, int nc, double matrix[nr][nc]) {
	for (int i = 0; i < nr; i++) {
    	for (int j = 0; j < nc; j ++) {
        	printf("%lf  ", matrix[i][j]);
        }
        printf("\n");
    }
}