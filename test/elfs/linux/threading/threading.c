#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* thread_func(void* arg)
{
    printf("Hello from thread 1!\n");
    return NULL;
}

int main()
{
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
    printf("Hello from thread 0!\n");
    pthread_join(thread, NULL); // Wait for the thread to terminate
    printf("Both threads finished.\n");
    return 0;
}
