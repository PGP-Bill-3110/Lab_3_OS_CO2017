#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_RESOURCES 3
#define NUM_PROCESSES 5

typedef struct {
    int id;                         
    int requested_resources;        
    void (*callback)(int);          
} process_request_t;

int available_resources = NUM_RESOURCES;
pthread_mutex_t resource_lock;
pthread_cond_t resource_cond;


void resource_callback(int process_id) {
    printf("Process %d: resource allocated!\n", process_id);
}


void* resource_manager(void* arg) {
    process_request_t* request = (process_request_t*) arg;

    pthread_mutex_lock(&resource_lock);
    
    while (request->requested_resources > available_resources) {
        printf("Process %d waiting for resources...\n", request->id);
        pthread_cond_wait(&resource_cond, &resource_lock);
    }

    
    available_resources -= request->requested_resources;
    request->callback(request->id);

    pthread_mutex_unlock(&resource_lock);

    
    sleep(1);

    
    pthread_mutex_lock(&resource_lock);
    available_resources += request->requested_resources;
    printf("Process %d released resources\n", request->id);
    pthread_cond_broadcast(&resource_cond);
    pthread_mutex_unlock(&resource_lock);

    return NULL;
}

int main() {
    pthread_t threads[NUM_PROCESSES];
    process_request_t requests[NUM_PROCESSES];

    pthread_mutex_init(&resource_lock, NULL);
    pthread_cond_init(&resource_cond, NULL);

    
    for (int i = 0; i < NUM_PROCESSES; i++) {
        requests[i].id = i;
        requests[i].requested_resources = (i % 2) + 1; 
        requests[i].callback = resource_callback;

        pthread_create(&threads[i], NULL, resource_manager, &requests[i]);
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&resource_lock);
    pthread_cond_destroy(&resource_cond);

    printf("All processes finished.\n");
    return 0;
}
