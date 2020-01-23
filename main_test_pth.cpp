#include "main.h"
#include <unistd.h>

using namespace std;

int nthreads = 10;

typedef struct bounds
{
    int threadnum;
    int begin;
    int end;
} bounds_t;

void *thread_func(void *arg)
{
    struct bounds *mybounds;
    mybounds = (struct bounds *)arg;
    printf("hello, i am %d thread, my bounds: %d and %d\n",
           mybounds->threadnum, mybounds->begin, mybounds->end);
    while (true) {
        printf ("s");
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid[nthreads];
    int ti;
    bounds_t *bounds_arg = (bounds_t *)
        malloc(sizeof(bounds_t) * nthreads);
    for (ti = 0; ti < nthreads; ti++)
    {
        bounds_arg[ti].threadnum = ti;
        bounds_arg[ti].begin = ti * 10 + 1;
        bounds_arg[ti].end = (ti + 1) * 10;
        pthread_create(&tid[ti], NULL, thread_func, &bounds_arg[ti]);
    }
    /* join all threads... */
    sleep (1);
    free(bounds_arg);
}
