#include <pthread.h>
#include <stdio.h>

void *PrintHello(void * id) {
    int i = 0;
    // for(; i < 3 ; i++)
    // {
        printf("Thread %d: Hello World!\n", (int)id );
    // }
    pthread_exit(NULL); /*not necessary*/
}

int main (int argc, char * argv[] )
{
    pthread_t thread0, thread1;
    pthread_create(&thread0, NULL, PrintHello, (void *) 0);
    pthread_create(&thread1, NULL, PrintHello, (void *) 1);
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
    return 0;
}