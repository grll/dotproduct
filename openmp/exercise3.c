#include <omp.h>
#include <stdio.h>

#define MAX_THREADS 16

static long num_steps = 100000000;
double step;

int main ()
{
    double sum = 0.0;
    double pi = 0.0;
    step = 1.0/(double)num_steps;
    double start_time, run_time;

    for (int j=1; j<=MAX_THREADS; j++)
    {
        sum = 0.0;
        omp_set_num_threads(j);
        start_time = omp_get_wtime();

        #pragma omp parallel
        {
            int thread_id = omp_get_thread_num();
            int num_threads = omp_get_num_threads();
            int steps_per_thread = num_steps / num_threads;

            // printf("Thread #%d: iterating from %d to %d.\n", thread_id, thread_id * steps_per_thread, (thread_id + 1) * steps_per_thread);
            int i;
            double x = 0.0;
            double local_sum = 0.0;
            for (i=thread_id * steps_per_thread;i<(thread_id + 1) * steps_per_thread;i++)
            {
                x = (i+0.5)*step;
                local_sum += 4.0 / (1.0+x*x);
            }

            #pragma omp atomic
            sum += local_sum;
        }
    
        pi = step * sum;
        
        run_time = omp_get_wtime() - start_time;
        printf("pi = %f, in %f seconds with %d threads \n", pi, run_time, j);
    }
}

