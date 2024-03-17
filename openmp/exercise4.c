#include <omp.h>
#include <stdio.h>

#define MAX_THREADS 16

static long num_steps = 100000000;
double step;

int main ()
{
    double pi = 0.0;
    double sum = 0.0;

    step = 1.0/(double)num_steps;
    double start_time, run_time;

    for (int j=1; j<=MAX_THREADS; j++)
    {
        sum = 0.0;
        omp_set_num_threads(j);
        start_time = omp_get_wtime();

        #pragma omp parallel for schedule(auto) reduction(+:sum)
        for (int i = 0;i<num_steps;i++)
        {
            double x = (i+0.5)*step;
            sum += 4.0 / (1.0+x*x);
        }
    
        pi = step * sum;
        
        run_time = omp_get_wtime() - start_time;
        printf("pi = %f, in %f seconds with %d threads \n", pi, run_time, j);
    }
}

