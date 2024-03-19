#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> // for random number generation

struct Bucket {
    int numElems;
    int index;
    int start;
};

void swapArrays(int **arr1, int **arr2) {
    int *temp = *arr1;
    *arr1 = *arr2;
    *arr2 = temp;
}

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int main() {

    int *arr, *sortedArr;
    int size, buckets, i, j, w;
    struct Bucket *bucketList;
    double start;
    float duration;

    printf("Enter the size of the array: \n");
    scanf("%d", &size);
    printf("Enter the number of buckets: \n");
    scanf("%d", &buckets);

    arr = (int *)malloc(sizeof(int) * size);
    sortedArr = (int *)malloc(sizeof(int) * size);

    bucketList = (struct Bucket *)calloc(buckets, sizeof(struct Bucket));

    int limit = 100000;
    w = limit / buckets;

    srand(time(NULL)); 

    for (i = 0; i < size; i++) {
        arr[i] = rand() % limit; 
    }

    // printf("Unsorted Array:\n");
    // for (i = 0; i < size; i++) {
    //     printf("%d ", arr[i]);
    // }
    // printf("\n");

    start = omp_get_wtime();

    w = limit / buckets; 
    for (i = 0; i < size; i++) {
        j = arr[i] / w;
        if (j > buckets - 1)
            j = buckets - 1;
        bucketList[j].numElems++;
    }

    for (i = 1; i < buckets; i++) {
        bucketList[i].index = bucketList[i - 1].index + bucketList[i - 1].numElems;
        bucketList[i].start = bucketList[i - 1].start + bucketList[i - 1].numElems;
    }

    int index;
    for (i = 0; i < size; i++) {
        j = arr[i] / w;
        if (j > buckets - 1)
            j = buckets - 1;
        index = bucketList[j].index++;
        sortedArr[index] = arr[i];
    }

    for (i = 0; i < buckets; i++)
        qsort(sortedArr + bucketList[i].start, bucketList[i].numElems, sizeof(int), compare);

    // printf("Sorted Array:\n");
    // for (i = 0; i < size; i++) {
    //     printf("%d ", sortedArr[i]);
    // }
    // printf("\n");

    swapArrays(&arr, &sortedArr);
    duration = omp_get_wtime() - start;

    printf("Sorting %d elements took %f seconds\n", size, duration);

    free(arr);
    free(sortedArr);
    free(bucketList);

    return 0;
}
