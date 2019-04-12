#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

#define MAX 10

//Global Variables
FILE *matrixInput;
FILE *matrixOutput;
int row1,row2,col1,col2;
int m1[MAX][MAX];
int m2[MAX][MAX];
int r [MAX][MAX];

struct my_thread_info{
    int ii;
    int jj;
    int col11;
    int col22;
};



//Main Functions
void ReadMatrices(void){

    int value;

    // Read matrix 1
    fscanf(matrixInput,"%d",&row1);
    fscanf(matrixInput,"%d",&col1);

    for(int i=0; i<row1; i++)
    {

        for(int j=0; j<col1; j++)
        {

            fscanf(matrixInput,"%d",&value);
            m1[i][j] = value;

        }
    }


    //Read Matrix 2
    fscanf(matrixInput,"%d",&row2);
    fscanf(matrixInput,"%d",&col2);

    for(int i=0; i<row1; i++)
    {

        for(int j=0; j<col1; j++)
        {

            fscanf(matrixInput,"%d",&value);
            m2[i][j] = value;

        }
    }

}

void StoreResult(int,double);

void *CalculateMatrixElement(void *data){

    struct my_thread_info *info = data;

   r[info->ii][info->jj] = 0;

    for(int k=0; k< info->col11; k++){

        r[info->ii][info->jj] += m1[info->ii][k] * m2[k][info->jj];

    }

}

void ComputeVersionA(int row1, int col1, int row2, int col2){

     clock_t start, end;
     double cpu_time_used;

     start = clock();

     struct my_thread_info *info;

    //make multiplication
    //computation of each element happens in seperate tread
    int q=0;
   pthread_t thread_id [row1*col2];
    pthread_t temp;

    for(int i=0; i<row1; i++)
    {

        for(int j=0; j<col2; j++)
        {

            //calculate each element in result matrix

            //assign new thread to each element
            info = malloc(sizeof(struct my_thread_info));
            info->ii = i;
            info->jj = j;
            info->col11 = col1;
            info->col22 = col2;

            pthread_create(&temp, NULL, CalculateMatrixElement, info);
            thread_id[q++] = temp;

        }
    }

    //wait for all threads to finish in order to proceed
    for(int i=0; i< row1*col2; i++){

        pthread_join(thread_id[i], NULL);

    }


    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


    StoreResult(1,cpu_time_used);

}

void *CalculateMatrixRow(void *data){

    struct my_thread_info *info = data;

    for(int j=0; j<info->col22; j++){

        r[info->ii][j] = 0;

        for(int k=0; k<info->col11; k++)
        {

            r[info->ii][j] += m1[info->ii][k] * m2[k][j];

        }

    }

}

void ComputeVersionB(int row1, int col1, int row2, int col2){

     clock_t start, end;
     double cpu_time_used;

     start = clock();

     struct my_thread_info *info;

    //make multiplication
    //computation of each row happens in seperate thread
    int q=0;
    pthread_t thread_id [row1];
    pthread_t temp;

    for(int i=0; i<row1; i++)
    {
        //calculate each row in result matrix

        //assign new thread to each element
        info = malloc(sizeof(struct my_thread_info));
        info->ii = i;
        info->jj = 0;
        info->col11 = col1;
        info->col22 = col2;

        pthread_create(&temp, NULL, CalculateMatrixRow, info);
        thread_id[q++] = temp;

    }

    //wait for all threads to finish in order to proceed
    for(int i=0; i< row1; i++){

        pthread_join(thread_id[i], NULL);

    }


    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


    StoreResult(2,cpu_time_used);


}

void ComputeVersionC(int row1, int col1, int row2, int col2){

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    //make multiplication
    //computation of each row happens in seperate tread

    for(int i=0; i<row1; i++)
    {

        for(int j=0; j<col2; j++)
        {

            r[i][j] = 0;

            for(int k=0; k<col1; k++)
            {

                r[i][j] += m1[i][k] * m2[k][j];

            }

        }
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


    StoreResult(3,cpu_time_used);


}

void StoreResult(int k,double t){

    //write results in output file

    fprintf(matrixOutput,"Result \n");

    for(int i=0; i<row1; i++)
    {

        for(int j=0; j<col1; j++)
        {
            fprintf(matrixOutput,"%d ",r[i][j]);

        }

        fprintf(matrixOutput,"\n");
    }


     fprintf(matrixOutput,"End %d  [%f sec]\n",k,t);


}



int main(){

    matrixInput = fopen("Input.txt","r");
    matrixOutput = fopen("Output.txt","w");


    ReadMatrices();
    ComputeVersionA(row1,col2,row2,col2);
    ComputeVersionB(row1,col2,row2,col2);
    //ComputeVersionC(row1,col2,row2,col2);

    fclose(matrixInput);
    fclose(matrixOutput);

    printf("Matrix Multiplication Done! \nCheck Output.txt for results...");

    return 0;
}
