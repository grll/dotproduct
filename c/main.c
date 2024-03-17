#include <math.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// this is redefined in generate_data.c probably better way to handle this
// with a single source of truth.
#define SIZE 1048576 // size of the array for the dot product
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

float fmadot(float x[], float w[])
{
    float a = 0.0;
    for (int i = 0; i < SIZE; i++)
    {
        a = fma(x[i], w[i], a);
    }
    return a;
}

/*
    * long n: the number of elements in x / y to process.
    * float * x: pointer to the x array in the heap.
    * float * y: pointer to the y array in the heap.
*/
float asm_dot(long i, long n, float x[], float y[])
{
    float dot[4] = {0.0, 0.0, 0.0, 0.0};

    // process floats 32 by 32
    __asm__ __volatile__
    (
    //reset all upper 128 bits of all YMM registers
    //for compatibility with old 128 bits SSE registers XMM
    "vzeroall					     \n\t"

    // reset to 0 registers YMM0 to YMM8 256 bits registers
	"vxorps		%%ymm0, %%ymm0, %%ymm0	             \n\t"
	"vxorps		%%ymm1, %%ymm1, %%ymm1	             \n\t"
	"vxorps		%%ymm2, %%ymm2, %%ymm2	             \n\t"
	"vxorps		%%ymm3, %%ymm3, %%ymm3	             \n\t"
	"vxorps		%%ymm4, %%ymm4, %%ymm4	             \n\t"
	"vxorps		%%ymm5, %%ymm5, %%ymm5	             \n\t"
	"vxorps		%%ymm6, %%ymm6, %%ymm6	             \n\t"
	"vxorps		%%ymm7, %%ymm7, %%ymm7	             \n\t"

    // ensure that the next piece of code or data is aligned
    // on a 2^4 = 16 bytes boundary next pieve of data begins
    // on memory address  that is a multiple of 16 bytes
    // make sure memory alignment before loops avoid some
    // cycle.
    ".p2align 4			             \n\t"
	"1:				             \n\t"
        // load 256 bits = 32 bytes = 8 floats from x
        "vmovups                  (%[x],%[i],4), %%ymm8          \n\t"  // 2 * x
        // load 256 bits = 32 bytes = 8 floats from x
        "vmovups                32(%[x],%[i],4), %%ymm9          \n\t"  // 2 * x

        // same load 16 floats from y
        "vmovups                  (%[y],%[i],4), %%ymm12         \n\t"  // 2 * y
        "vmovups                32(%[y],%[i],4), %%ymm13         \n\t"  // 2 * y

        // load next 16 floats from x
        "vmovups                64(%[x],%[i],4), %%ymm10         \n\t"  // 2 * x
        "vmovups                96(%[x],%[i],4), %%ymm11         \n\t"  // 2 * x

        // load next 16 floats from y
        "vmovups                64(%[y],%[i],4), %%ymm14         \n\t"  // 2 * y
        "vmovups                96(%[y],%[i],4), %%ymm15         \n\t" 

        // perform fused multiply-add
        // 231 = 2*3 + 1, ps for packed single precision
        // multiply 8 floats from y in ymm12 with 8 float
        // from x in ymm8. sum with ymm0 and store result there
        "vfmadd231ps       %%ymm8, %%ymm12, %%ymm0            \n\t"
        "vfmadd231ps       %%ymm9 , %%ymm13, %%ymm1     \n\t"
        "vfmadd231ps       %%ymm10 , %%ymm14, %%ymm2     \n\t"
        "vfmadd231ps       %%ymm11 , %%ymm15, %%ymm3     \n\t"

        "addq $32, %[i] \n\t"
        "cmpq %[n], %[i] \n\t"
        "jne 1b \n\t"

	    "vaddps        %%ymm0, %%ymm1, %%ymm0	\n\t"
	    "vaddps        %%ymm2, %%ymm3, %%ymm2	\n\t"
	    "vaddps        %%ymm0, %%ymm2, %%ymm0	\n\t"

        "vextractf128 $1 , %%ymm0 , %%xmm1	\n\t"

        "vaddps        %%xmm0, %%xmm1, %%xmm0	\n\t"

        "vmovups       %%xmm0,    (%[dot])		\n\t"
        "vzeroall \n\t"
    :
        [i]"+r" (i)
    :
        [n]"r"  (n),
        [x] "r" (x),
        [y] "r" (y),
        [dot] "r" (dot)
    :
        "cc",
        "%xmm0", "%xmm1", "%xmm2", "%xmm3", 
	    "%xmm4", "%xmm5", "%xmm6", "%xmm7", 
	    "%xmm8", "%xmm9", "%xmm10", "%xmm11",
	    "%xmm12", "%xmm13", "%xmm14", "%xmm15",
	    "memory"
    );
    return dot[0] + dot[1] + dot[2] + dot[3];
}

int main()
{
    // load the data from file generated with generate_data.c
    FILE *fptr;
    fptr = fopen("data.binary", "rb");
    float * x = malloc(SIZE * sizeof(float));
    if (fread(x,sizeof(float),SIZE,fptr) != SIZE) {
        printf("Failed to read x from file");
        return 1;
    }
    float * w = malloc(SIZE * sizeof(float));
    if (fread(w,sizeof(float),SIZE,fptr) != SIZE) {
        printf("Failed to read w from file");
        return 1;
    }
    fclose(fptr);

    // perform the dot product sim times.
    clock_t start, finish;
    double duration;

    double start_time, end_time;
    float a;
    start = clock();
    start_time = omp_get_wtime();
    for (int i = 0; i < SIM; i++)
    {
        a = 0.0;
        #pragma omp parallel for reduction(+:a)
        for (int j=0; j<SIZE; j+=SIZE/32)
        {
            a += asm_dot(j,j+SIZE/32, x, w);
        }
    }
    end_time = omp_get_wtime();
    finish = clock();
    duration = (double)(finish - start) / (CLOCKS_PER_SEC * SIM);
    double omp_duration = (end_time - start_time) / SIM;

    printf("ASM -- The result for a is: %f\n", a);
    printf("ASM -- %.9f seconds\n", duration);
    printf("ASM -- %.9f omp seconds\n", omp_duration);

    start = clock();
    for (int i = 0; i < SIM; i++)
    {
        a = dot(x, w);
    }
    finish = clock();
    duration = (double)(finish - start) / (CLOCKS_PER_SEC * SIM);

    printf("C   -- The result for a is: %f\n", a);
    printf("C   -- %.9f seconds\n", duration);

    start = clock();
    for (int i = 0; i < SIM; i++)
    {
        a = fmadot(x, w);
    }
    finish = clock();
    duration = (double)(finish - start) / (CLOCKS_PER_SEC * SIM);

    printf("FMA -- The result for a is: %f\n", a);
    printf("FMA -- %.9f seconds\n", duration);

    free(x);
    free(w);
    return 0;
}

