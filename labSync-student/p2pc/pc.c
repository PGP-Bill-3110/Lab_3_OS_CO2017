#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>


#define BUF_SIZE 2
#define THREADS 1 // 1 producer and 1 consumer
#define LOOPS 3 * BUF_SIZE // variable

// Initiate shared buffer
int buffer[BUF_SIZE];
int fill = 0;
int use = 0;

/*TODO: Fill in the synchronization stuff */

sem_t empty; // counting semaphore for empty slots
sem_t full;  // counting semaphore for full slots 
sem_t mutex; // binary semaphore for mutual exclusion

void put(int value); // put data into buffer
int get();           // get data from buffer

void * producer(void * arg) {
  int i;
  int tid = *((int*) arg);
  for (i = 0; i < LOOPS; i++) {
    /*TODO: Fill in the synchronization stuff */
    sem_wait(&empty); // wait for empty slot
    sem_wait(&mutex); // enter critical section
    put(i); // line P2
    sem_post(&mutex); // leave critical section
    sem_post(&full);  // signal that a new item is available
    printf("Producer %d put data %d\n", tid, i);
    sleep(1);
    /*TODO: Fill in the synchronization stuff */
  }
  sem_wait(&empty); // wait for empty slot
  sem_wait(&mutex); // enter critical section
  put(-1); // put termination signal
  sem_post(&mutex); // leave critical section
  sem_post(&full);  // signal that a new item is available

  pthread_exit(NULL);
}

void * consumer(void * arg) {
  int tid = *((int*) arg);
  int tmp = 0;

  while (tmp != -1) {

    sem_wait(&full);   
    sem_wait(&mutex);  
    tmp = get();       

    sem_post(&mutex);  
    sem_post(&empty);  

    printf("Consumer %d get data %d\n", tid, tmp);
    sleep(1);
  }

  pthread_exit(NULL);
}


int main(int argc, char ** argv) {
  int i, j;
  int tid[THREADS];
  pthread_t producers[THREADS];
  pthread_t consumers[THREADS];

  /*TODO: Fill in the synchronization stuff */

  sem_init(&empty, 0, BUF_SIZE); 
  sem_init(&full, 0, 0);         
  sem_init(&mutex, 0, 1);        

  for (i = 0; i < THREADS; i++) {
    tid[i] = i;
 
    pthread_create( & producers[i], NULL, producer, (void * ) &tid[i]);


    pthread_create( & consumers[i], NULL, consumer, (void * ) &tid[i]);
  }

  for (i = 0; i < THREADS; i++) {
    pthread_join(producers[i], NULL);
    pthread_join(consumers[i], NULL);
  }

  /*TODO: Fill in the synchronization stuff destroy (if needed) */
sem_destroy(&empty);
sem_destroy(&full);
sem_destroy(&mutex);

  return 0;
}

void put(int value) {
  buffer[fill] = value;          // line f1
  fill = (fill + 1) % BUF_SIZE; // line f2
}

int get() {
  int tmp = buffer[use];         // line g1
  buffer[use] = -1;              //clean the item
  use = (use + 1) % BUF_SIZE;   // line g2
  return tmp;
}

//   gcc p2pc/pc.c -o p2pc/pc -pthread 
// ./p2pc/pc

