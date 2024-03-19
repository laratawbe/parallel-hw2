#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h> 

struct Bucket {
    int numElems;
    int* elements;
};

struct ThreadData {
    int* array;
    struct Bucket* bucket;
};

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void* sortBucket(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    qsort(data->bucket->elements, data->bucket->numElems, sizeof(int), compare);
    return NULL;
}

int main() {
    int *arr, size, buckets, limit = 100000;
    struct Bucket *bucketList;
    pthread_t* threads;
    struct ThreadData* threadData;

    printf("Give length of array to sort: ");
    if (scanf("%d", &size) != 1) {
        printf("Error in input\n");
        return -1;
    }

    printf("Give number of buckets: ");
    scanf("%d", &buckets);

    arr = (int*)malloc(size * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % limit;
    }

    bucketList = (struct Bucket*)calloc(buckets, sizeof(struct Bucket));
    threads = (pthread_t*)malloc(buckets * sizeof(pthread_t));
    threadData = (struct ThreadData*)malloc(buckets * sizeof(struct ThreadData));

    for (int i = 0; i < size; i++) {
        int index = arr[i] / (limit / buckets);
        if (index >= buckets) index = buckets - 1;
        bucketList[index].numElems++;
    }

    for (int i = 0; i < buckets; i++) {
        bucketList[i].elements = (int*)malloc(bucketList[i].numElems * sizeof(int));
        bucketList[i].numElems = 0;
    }

    for (int i = 0; i < size; i++) {
        int index = arr[i] / (limit / buckets);
        if (index >= buckets) index = buckets - 1;
        bucketList[index].elements[bucketList[index].numElems++] = arr[i];
    }

    double start_time = omp_get_wtime(); 

    for (int i = 0; i < buckets; i++) {
        threadData[i].array = arr;
        threadData[i].bucket = &bucketList[i];
        pthread_create(&threads[i], NULL, sortBucket, &threadData[i]);
    }

    for (int i = 0; i < buckets; i++) {
        pthread_join(threads[i], NULL);
    }

    int index = 0;
    for (int i = 0; i < buckets; i++) {
        for (int j = 0; j < bucketList[i].numElems; j++) {
            arr[index++] = bucketList[i].elements[j];
        }
        free(bucketList[i].elements);
    }

    double end_time = omp_get_wtime(); 
    double total = end_time - start_time;

    // printf("Sorted Array:\n");
    // for (int i = 0; i < size; i++) {
    //     printf("%d ", arr[i]);
    // }
    // printf("\n");

    printf("Sorting %d elements took %.6f seconds\n", size, total); 
    free(bucketList);
    free(threads);
    free(threadData);
    free(arr);

    return 0;
}
