/* A C program to generate data for benchmarking. */
#include <stdlib.h>
#include <stdio.h>

#define SIZE 1000000

int main()
{
    srand(42); // seed the random number

    // generate the array of float
    float x[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        x[i] = (float)rand() / ((float)RAND_MAX / 2.0) - 1.0;
    }
    float w[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        w[i] = (float)rand() / ((float)RAND_MAX / 2.0) - 1.0;
    }

    // write the array as binary to file
    FILE *fptr;
    fptr = fopen("data.binary", "wb");
    fwrite(x,sizeof(float),SIZE,fptr);
    fwrite(w,sizeof(float),SIZE,fptr);
    fclose(fptr);

    return 0;
}
