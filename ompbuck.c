#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define max_threads 80

struct Bucket {
    int numElems;
    int index; // [start : numElems)
    int start; // starting point in B array
};

int compare(const void * a, const void * b) {
    return (*(int*)a - *(int*)b);
}

int main(int argc, char *argv[]) {

    int *A, *B, *temp;
    int size, numBuckets, i, w, limit, numThreads, workload, bucketIndex;
    struct Bucket* buckets;
    double start;
    float total;

    printf("Give length of array to sort \n");
    if (scanf("%d", &size) != 1) {
        printf("error\n");
        return -1;
    }

    printf("Give number of buckets \n");
    scanf("%d", &numBuckets);

    int globalNumElems[numBuckets];
    int globalStartingPosition[numBuckets];
    memset(globalNumElems, 0, sizeof(int) * numBuckets);
    memset(globalStartingPosition, 0, sizeof(int) * numBuckets);

    numThreads = numBuckets;
    omp_set_num_threads(numThreads);

    limit = 100000;
    w = limit / numBuckets;
    A = (int *)malloc(sizeof(int) * size);
    B = (int *)malloc(sizeof(int) * size);

    for (i = 0; i < size; i++) {
        A[i] = random() % limit;
    }

    // printf("Unsorted array of %d elements:\n", size);
    // for (i = 0; i < size; i++) {
    //     printf("%d ", A[i]);
    // }
    // printf("\n");

    buckets = (struct Bucket *)calloc(numBuckets * numThreads, sizeof(struct Bucket));

    start = omp_get_wtime();

    #pragma omp parallel
    {
        numThreads = omp_get_num_threads();
        int j, k;
        int localIndex; // [0 : numBuckets)
        int realBucketIndex; // [0 : numBuckets * numThreads)
        int myID = omp_get_thread_num();
        workload = size / numThreads;

        #pragma omp for private(i, localIndex)
        for (i = 0; i < size; i++) {
            localIndex = A[i] / w;
            if (localIndex > numBuckets - 1)
                localIndex = numBuckets - 1;
            realBucketIndex = localIndex + myID * numBuckets;
            buckets[realBucketIndex].numElems++;
        }

        int localSum = 0;
        for (j = myID; j < numBuckets * numThreads; j = j + numThreads) {
            localSum += buckets[j].numElems;
        }
        globalNumElems[myID] = localSum;

        #pragma omp barrier

        #pragma omp master
        {
            for (j = 1; j < numBuckets; j++) {
                globalStartingPosition[j] = globalStartingPosition[j - 1] + globalNumElems[j - 1];
                buckets[j].start = buckets[j - 1].start + globalNumElems[j - 1];
                buckets[j].index = buckets[j - 1].index + globalNumElems[j - 1];
            }
        }

        #pragma omp barrier

        for (j = myID + numBuckets; j < numBuckets * numThreads; j = j + numThreads) {
            int previousIndex = j - numBuckets;
            buckets[j].start = buckets[previousIndex].start + buckets[previousIndex].numElems;
            buckets[j].index = buckets[previousIndex].index + buckets[previousIndex].numElems;
        }

        #pragma omp barrier

        #pragma omp for private(i, bucketIndex)
        for (i = 0; i < size; i++) {
            j = A[i] / w;
            if (j > numBuckets - 1)
                j = numBuckets - 1;
            k = j + myID * numBuckets;
            bucketIndex = buckets[k].index++;
            B[bucketIndex] = A[i];
        }

        #pragma omp for private(i)
        for (i = 0; i < numBuckets; i++)
            qsort(B + globalStartingPosition[i], globalNumElems[i], sizeof(int), compare);
    }

    total = omp_get_wtime() - start;
    temp = A;
    A = B;
    B = temp;

    // printf("Sorted array of %d elements:\n", size);
    // for (i = 0; i < size; i++) {
    //     printf("%d ", A[i]);
    // }
    // printf("\n");
    printf("Sorting %d elements took %.6f seconds\n", size, total);

    return 0;
}
