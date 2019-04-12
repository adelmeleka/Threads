#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <time.h>

#define MAX 100

FILE *arrayInput;
FILE *arrayOutput;
int arr_size;
int arr[MAX];

typedef struct my_thread_info{
    int start;
    int end;
}INFO;

//Main Functions
void ReadArray(void){

    int value;

    // Read array size
    fscanf(arrayInput,"%d",&arr_size);

    for(int i=0; i<arr_size; i++)
    {

        fscanf(arrayInput,"%d",&value);
        arr[i] = value;

    }

}

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge( int l, int m, int r){

    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}


void *mergeSort(void *info){

   INFO info1 , info2;
    INFO *info3 = (INFO *)info;
    pthread_t thread_id1;
    pthread_t thread_id2;
    int ret;
    if ((info3->start) < (info3->end))
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = ((info3->start)+((info3->end)))/2;

        //info1 = malloc(sizeof(struct my_thread_info));

            info1.start = info3->start;
            info1.end = m;


         ret = pthread_create(&thread_id1, NULL, mergeSort, &info1);
        if (ret) {
                printf("%d %s - unable to create thread - ret - %d\n", __LINE__, __FUNCTION__, ret);
                exit(1);
            }
        // info1 = malloc(sizeof(struct my_thread_info));

            info2.start = m+1;
            info2.end = info3->end;

         ret = pthread_create(&thread_id2, NULL, mergeSort, &info2);
        if (ret) {
                printf("%d %s - unable to create thread - ret - %d\n", __LINE__, __FUNCTION__, ret);
                exit(1);
            }
         pthread_join(thread_id1, NULL);
         pthread_join(thread_id2, NULL);

        // Sort first and second halves
        //mergeSort(arr, l, m);
        //mergeSort(arr, m+1, r);

         merge(info3->start, m, info3->end);
    }
}

void StoreResult(double t){

    //write results in output file

    fprintf(arrayOutput,"Result \n");

    for(int i=0; i<arr_size; i++)
    {

            fprintf(arrayOutput,"%d ",arr[i]);

    }


     fprintf(arrayOutput,"\nTime Taken [%f sec]\n",t);


}


void sort(){

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    INFO info;
            info.start = 0;
            info.end = arr_size - 1;
            pthread_t tid;
            int ret;
          ret = pthread_create(&tid, NULL, mergeSort, &info);
    if (ret) {
                printf("%d %s - unable to create thread - ret - %d\n", __LINE__, __FUNCTION__, ret);
                exit(1);
            }
            pthread_join(tid, NULL);


    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    StoreResult(cpu_time_used);


}



int main(){

    arrayInput = fopen("Input.txt","r");
    arrayOutput = fopen("Output.txt","w");

    ReadArray();

    sort();

    fclose(arrayInput);
    fclose(arrayOutput);

    printf("Merge Sort Done! \nCheck Output.txt for results...");

    return 0;
}
