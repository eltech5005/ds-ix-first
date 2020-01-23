#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

const int n = 30;
int a[n]; //n - количество элементов

const int max_threads = sysconf(_SC_NPROCESSORS_ONLN);
int thread_counter = 0;

pthread_mutex_t lock;

struct qs_args
{
    int *s_arr;
    int first;
    int last;
};

// --------

void *qs(void *arguments)
{

    pthread_t pthread_self(void);
    pthread_t tid = pthread_self();

    struct qs_args *args = (qs_args *)arguments;
    int first = args->first;
    int last = args->last;
    int *s_arr = args->s_arr;

    struct qs_args elem[2];
    pthread_t th[2];
    int thi[2] = {0};
    int inter_couter;

    if (first < last)
    {
        int left = first, right = last, middle = s_arr[(left + right) / 2];
        // int left = first, right = last, middle = left;

printf("[%d] Start QS\n with first: %d,, middle: %d last: %d\n", tid, first+1, middle+1, last+1);
for (int i = 0; i < n; i++)
    {
        printf("%d ", s_arr[i]);
    }
    puts("");


        do
        {
            while (s_arr[left] < middle)
                left++;
            while (s_arr[right] > middle)
                right--;
            if (left <= right)
            {
                int tmp = s_arr[left];
                s_arr[left] = s_arr[right];
                s_arr[right] = tmp;
                left++;
                right--;
            }

        
        } while (left <= right);

        elem[0].s_arr = s_arr;
        elem[0].first = first;
        elem[0].last = right;
        elem[1].s_arr = s_arr;
        elem[1].first = left;
        elem[1].last = last;



        printf("[%d] Lock mutex\n", tid);
        pthread_mutex_lock(&lock);
        for (inter_couter = 2; (inter_couter > 0) && (thread_counter + 1 < max_threads); inter_couter--)
        {
                        thread_counter++;
            printf("[%d] Spawn thread number %d of %d max\n", tid, thread_counter + 1, max_threads);
            pthread_create(&th[inter_couter - 1], NULL, &qs, (void *)&elem[inter_couter - 1]);
            thi[inter_couter - 1] = 1;
        }
        pthread_mutex_unlock(&lock);
        printf("[%d] Unlocked mutex\n", tid);

        for (; inter_couter > 0; inter_couter--)
        {
            printf("[%d] No available threads: %d of %d - fall into recurse\n", tid, thread_counter + 1, max_threads);
            qs((void *)&elem[inter_couter - 1]);
        }

        for (int i = 0; i < 2; i++)
        {
            if (thi[i] == 1)
            {
                printf("[%d] Join child thread %d\n", tid, i);
                pthread_join(th[i], NULL);
            }
        }
        printf("[%d] Exit\n", tid);
        return NULL;
    }
}

int main(int argc, char *argv[], char *envp[])
{
    srand(time(NULL));   // Initialization, should only be called once.
    for (int i = 0; i < n; i++)
    {
        a[i] = rand() % 100; // Returns a pseudo-random integer between 0 and RAND_MAX.
    }

    struct qs_args arguments;
    arguments.s_arr = a;
    arguments.first = 0;
    arguments.last = n - 1;
    pthread_mutex_init(&lock, NULL);

    puts("Initial array:");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", a[i]);
    }
    puts("");

    qs(&arguments);

    puts("Sorted array:");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", a[i]);
    }
    puts("");
}
