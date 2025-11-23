#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

pthread_mutex_t lock;
bool finished = false;


int issafe() {

    int found = rand() % 2; 
    if (!found) {
        return -1; 
    }
    return 0; 
}


void* periodical_detector(void* arg) {
    while (1) {
        sleep(5); 

        pthread_mutex_lock(&lock);
        if (!issafe()) {
            printf("Unsafe detected! Taking corrective action...\n");
        }

        if (finished) {
            pthread_mutex_unlock(&lock);
            break; 
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t detector;

    srand(time(NULL));
    pthread_mutex_init(&lock, NULL);

  
    pthread_create(&detector, NULL, periodical_detector, NULL);


    printf("Main program running... press Enter to finish.\n");
    getchar(); 

    pthread_mutex_lock(&lock);
    finished = true;
    pthread_mutex_unlock(&lock);

    pthread_join(detector, NULL);
    pthread_mutex_destroy(&lock);

    printf("Program terminated safely.\n");
    return 0;
}
