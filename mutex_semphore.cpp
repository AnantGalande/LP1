#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
using namespace std;

#define N 5
int buffer[N], in = 0, out = 0;
pthread_mutex_t mtx;
sem_t *full, *empty;

void* producer(void*) {
    for (int item = 1; item <= N; item++) {
        sem_wait(empty);
        pthread_mutex_lock(&mtx);

        buffer[in] = item;
        cout << "Producer produced item " << item << " at position " << in << endl;
        in = (in + 1) % N;

        pthread_mutex_unlock(&mtx); 
        sem_post(full);
        sleep(1);
    }
    return 0;
}

void* consumer(void*) {
    for (int i = 1; i <= N; i++) {
        sem_wait(full);
        pthread_mutex_lock(&mtx);

        int item = buffer[out];
        cout << "\tConsumer consumed item " << item << " from position " << out << endl;
        out = (out + 1) % N;

        pthread_mutex_unlock(&mtx);
        sem_post(empty);
        sleep(1);
    }
    return 0;
}

int main() {
    pthread_t p, c;
    pthread_mutex_init(&mtx, 0);

    sem_unlink("/full");
    sem_unlink("/empty");
    full  = sem_open("/full",  O_CREAT, 0644, 0);
    empty = sem_open("/empty", O_CREAT, 0644, N);


    pthread_create(&p, 0, producer, 0);
    pthread_create(&c, 0, consumer, 0);
    pthread_join(p, 0);
    pthread_join(c, 0);

    pthread_mutex_destroy(&mtx);
    sem_close(full); sem_close(empty);
    sem_unlink("/full"); sem_unlink("/empty");

    cout << "\nExecution completed for " << N << " items." << endl;
}
