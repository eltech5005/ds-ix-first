#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

// Array to sort
const int n = 100;
int a[n];

const int max_threads = sysconf(_SC_NPROCESSORS_ONLN); // Spawn max threads as much as logicals CPUs on host
int thread_counter = 0; // Global threads counter
pthread_mutex_t lock; // Mutex to make threads counter thread-safe

// Struct to pass arguments to threads
struct qs_args {
    int *s_arr;
    int first;
    int last;
};

// Recursive quick sort, threads spawning function
void *qs(void *arguments) {

    // Data to process
    struct qs_args *args = (qs_args *)arguments;
    int first = args->first;
    int last = args->last;
    int *s_arr = args->s_arr;

    // Service variables
    struct qs_args elem[2]; // Arguments to pass to spawned threads
    pthread_t th[2]; // Threads array
    int thi[2] = {0}; // Initialized threads flags
    int inter_couter; // Processed quick sort intervals counter

    // Main routine
    if (first < last) {

        // Quick sort itself
        int left = first, right = last, pivot = first;
        do {
            while (s_arr[left] <= s_arr[pivot] && left < last)
                left++;
            while (s_arr[right] > s_arr[pivot])
                right--;
            if (left < right) {
                int tmp = s_arr[left];
                s_arr[left] = s_arr[right];
                s_arr[right] = tmp;
            }
        } while (left < right);

        int tmp = s_arr[pivot];
        s_arr[pivot] = s_arr[right];
        s_arr[right] = tmp;

        // Create argument structures for additional threads
        elem[0].s_arr = s_arr;
        elem[0].first = first;
        elem[0].last = right-1;
        elem[1].s_arr = s_arr;
        elem[1].first = left;
        elem[1].last = last;

        // Check if we can spawn more threads and run QS for left and right intervals
        pthread_mutex_lock(&lock);
        for (inter_couter = 2; (inter_couter > 0) && (thread_counter + 1 < max_threads); inter_couter--) {
            thread_counter++;
            pthread_create(&th[inter_couter - 1], NULL, &qs, (void *)&elem[inter_couter - 1]);
            thi[inter_couter - 1] = 1; // Mark interval as processed
        }
        pthread_mutex_unlock(&lock);

        // If there are unprocessed intervals - max threads number reached
        // Preform classic recurse QS in this thread for all intervals left
        for (; inter_couter > 0; inter_couter--)
            qs((void *)&elem[inter_couter - 1]);

        // Join all spawned threads to let them finish jobs
        for (int i = 0; i < 2; i++)
            if (thi[i] == 1)
                pthread_join(th[i], NULL);

        return NULL;
    }
}

int main(int argc, char *argv[], char *envp[]) {

    srand(time(NULL)); // Random numbers generator initialization
    
    for (int i = 0; i < n; i++) // Fill test array with random numbers
        a[i] = rand() % 1000;

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

    return 0;
}
