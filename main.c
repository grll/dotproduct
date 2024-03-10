#include <time.h>
#include <stdio.h>

// this is redefined in generate_data.c probably better way to handle this
// with a single source of truth.
#define SIZE 1000000 // size of the array for the dot product
#define SIM 100 // number of simulation to perform

float dot(float x[], float w[])
{
    float a = 0.0;
    for (int i = 0; i < SIZE; i++)
    {
        a += x[i] * w[i];
    }
    return a;
}

int main()
{
    // load the data from file generated with generate_data.c
    FILE *fptr;
    fptr = fopen("data.binary", "rb");
    float x[SIZE];
    if (fread(x,sizeof(float),SIZE,fptr) != SIZE) {
        printf("Failed to read x from file");
        return 1;
    }
    float w[SIZE];
    if (fread(w,sizeof(float),SIZE,fptr) != SIZE) {
        printf("Failed to read w from file");
        return 1;
    }
    fclose(fptr);

    // perform the dot product sim times.
    clock_t start, finish;
    double duration;

    float a;
    start = clock();
    for (int i = 0; i < SIM; i++)
    {
        a = dot(x, w);
    }
    finish = clock();
    duration = (double)(finish - start) / (CLOCKS_PER_SEC * SIM);

    printf("The result for a is: %f\n", a);
    printf( "%.9f seconds\n", duration);

    return 0;
}
