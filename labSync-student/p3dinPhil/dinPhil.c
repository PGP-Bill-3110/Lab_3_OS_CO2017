#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

pthread_mutex_t mtx;
pthread_cond_t chopstick[N];

int fork_available[N];   // trạng thái của nĩa: 1 = free, 0 = taken

void *philosopher(void*);
void eat(int);
void think(int);

int main()
{
    int i, a[N];
    pthread_t tid[N];

    pthread_mutex_init(&mtx, NULL);

    for (i = 0; i < N; i++) {
        pthread_cond_init(&chopstick[i], NULL);
        fork_available[i] = 1;   // ban đầu nĩa rảnh
    }

    for (i = 0; i < 5; i++) {
        a[i] = i;
        pthread_create(&tid[i], NULL, philosopher, (void*) &a[i]);
    }

    for (i = 0; i < 5; i++)
        pthread_join(tid[i], NULL);

    return 0;
}

void *philosopher(void *num)
{
    int phil = *(int*) num;
    int left = phil;
    int right = (phil + 1) % N;

    printf("Philosopher %d has entered room\n", phil);

    while (1)
    {
        pthread_mutex_lock(&mtx);

        
        while (!fork_available[left])
            pthread_cond_wait(&chopstick[left], &mtx);

        fork_available[left] = 0;

        
        while (!fork_available[right])
            pthread_cond_wait(&chopstick[right], &mtx);

        fork_available[right] = 0;

        pthread_mutex_unlock(&mtx);

        printf("Philosopher %d takes fork %d and %d\n",
               phil, left, right);

        eat(phil);
        sleep(2);

        printf("Philosopher %d puts fork %d and %d down\n",
               phil, right, left);

        pthread_mutex_lock(&mtx);

      
        fork_available[left] = 1;
        fork_available[right] = 1;

        pthread_cond_signal(&chopstick[left]);
        pthread_cond_signal(&chopstick[right]);

        pthread_mutex_unlock(&mtx);

        think(phil);
        sleep(1);
    }
}

void eat(int phil)
{
    printf("Philosopher %d is eating\n", phil);
}

void think(int phil)
{
    printf("Philosopher %d is thinking\n", phil);
}
