#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <omp.h>
#include <pthread.h>

// Helpers
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


// Arg struct
typedef struct {
    long TID;
    int n;
    double* rowA;
    double* B;
    double* rowC;
} ThreadData;

// Pthread func
void* posixPrint(void* recStruct) {
    ThreadData* thisThreadData = (ThreadData*) recStruct;
    long TID = thisThreadData->TID;
    int n = thisThreadData->n;
    double* rowA = thisThreadData->rowA;
    double* B = thisThreadData->B;
    double* rowC = thisThreadData->rowC;

    // printf("\tin thread #%li and has n = %i\n", TID, n);
    // printf("\t");
    // for (int i = 0; i < n; i++) {
    //     printf("    %f",rowA[i]);
    // }
    // printf("\n\n\t");
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         printf("    %f",B[i*n + j]);
    //     }
    //     printf("\n\t");
    // }
    // printf("\n");
    //
    for (int i = 0; i < n; i++) {       // runs for rowA and B
        for (int j = 0; j < n; j++) {   // runs for B, COLUMN-wise
            rowC[i] += rowA[i] * B[i + j*n];
        }
    }
    //
    // printf("\tRow C:\n\t");
    // for (int i = 0; i < n; i++) {
    //     printf("    %f",rowC[i]);
    // }
    // printf("\n\n");

    pthread_exit(NULL);
}


// Main
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

    const int N_THREADS = n;
    pthread_t threadList[N_THREADS];
    pthread_attr_t attr;
    int createFlag = 0, joinFlag;
    void* status;


    double time = omp_get_wtime();


    // ======= PARALLELIZABLE ===============================
    // C = A x B
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (long idx = 0; idx < N_THREADS; idx++) {
        // idx = current row
        //printf("    Thread #%li is being created...\n",idx);

        // Prepare the ThreadData before passing
        ThreadData* threadData = new ThreadData;
        threadData->TID  = idx;
        threadData->n    = n;
        // threadData->rowA = (double*) malloc(sizeof(double)*n);
        // memcpy(threadData->rowA, &(A[idx*n]), sizeof(double)*n);
        // threadData->B    = (double*) malloc(sizeof(double)*n*n);
        // memcpy(threadData->B, B, sizeof(double)*n*n);
        // threadData->rowC = (double*) malloc(sizeof(double)*n);
        // memcpy(threadData->rowC, &(C[idx*n]), sizeof(double)*n);
        threadData->rowA = &(A[idx*n]);
        threadData->B    = B;
        threadData->rowC = &(C[idx*n]);


        createFlag = pthread_create(&threadList[idx], NULL, posixPrint, (void*) threadData);

        if (createFlag) {
            printf("Error! Return code from pthread_create() is %i\n", createFlag);
            exit(-1);
        }


    }

    pthread_attr_destroy(&attr);

    for (long idx = 0; idx < N_THREADS; idx++) {
        joinFlag = pthread_join(threadList[idx], &status);
        if (joinFlag) {
            printf("ERROR: return code from pthread_join() is %d\n", joinFlag);
            exit(-1);
        }
        //printf("Main: completed join with thread #%li having a status of %ld\n", idx, (long)status);
    }

    //printf("Main: program finished\n");
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

    // printArr(C, n);

    printf("Computing time: %f\n", time);


    pthread_exit(NULL);

}
