#include <cstdio>
#include <cstdlib>
#include <omp.h>

//#define OMP_NUM_THREADS 12
#define CHUNK_SIZE 100

void printArr(double* arr, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%f ", arr[i*n + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print(double* A, double* B, double* C, int n) {
    printArr(A,n);
    printArr(B,n);
    printArr(C,n);
}


int main(int argc, char** argv) {
    const int n = atoi(argv[1]);
        // This problem scales as n^3.
        // This value may need to be adjusted


    /* Init matrices
       This is a 1D array!!!
       Every n elements = 1 row
       has n rows */

    double* A = (double*) malloc(sizeof(double)*n*n);
    double* B = (double*) malloc(sizeof(double)*n*n);
    double* C = (double*) malloc(sizeof(double)*n*n);


    // CilkPlus array notation
    for (int i = 0 ; i < n; i++) {
        for(int j = 0; j < n; j++) {
            A[i*n + j] = (double)i / (double)n;
            B[i*n + j] = (double)j / (double)n;
            C[i*n + j] = 0.0;
        }
    }


    // print(A,B,C,n);

    printf("Carrying out matrix-matrix multiplication\n");



    int i,j,k;
    // printf("OMP_NUM_THREADS = %d\n",OMP_NUM_THREADS);
    // omp_set_dynamic(0);
    omp_set_num_threads(omp_get_num_procs());
    double time = omp_get_wtime();

    //printf("max threads = %d\n",omp_get_num_threads());

    // ======= PARALLELIZABLE ===============================
    // C = A x B
    #pragma omp parallel for private(i,j,k) shared(A,B,C)
    {
        for (i = 0; i < n; i++) {
            for (k = 0; k < n; k++) {
                for (j = 0; j < n; j++) {
                    C[i*n + j] += A[i*n + k] * B[k*n + j];
                }
            }
        }
    }

    // ====== END PARALLELIZABLE ============================


    time = omp_get_wtime() - time;



    printf("Checking the results...\n");
    double norm = 0.0;
    for ( int i = 0 ; i < n ; i++)
        for ( int j = 0 ; j < n ; j++)
            norm += (C[i*n+j]-(double)(i*j)/(double)n)*(C[i*n+j]-(double)(i*j)/(double)n);



    if (norm > 1e-10)
        printf("Something is wrong... Norm is equal to %f\n", norm);
    else
        printf("Yup, we're good!\n");

    printf("Computing time: %f\n", time);

    // printArr(C, n);

}
