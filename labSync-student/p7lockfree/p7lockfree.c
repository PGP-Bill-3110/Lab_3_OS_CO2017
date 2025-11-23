#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

typedef struct {
    _Atomic(Node*) head;
} LockFreeStack;


bool push(LockFreeStack* stack, int value) {
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) return false;
    new_node->value = value;

    Node* old_head;
    do {
        old_head = atomic_load(&stack->head);
        new_node->next = old_head;
    
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, new_node));

    return true;
}


bool pop(LockFreeStack* stack, int* value) {
    Node* old_head;
    Node* next_node;
    do {
        old_head = atomic_load(&stack->head);
        if (old_head == NULL) return false; 
        next_node = old_head->next;
        
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, next_node));

    *value = old_head->value;
    free(old_head);
    return true;
}

#define NUM_THREADS 4
#define NUM_OPS 10

LockFreeStack stack;

void* worker_push(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_OPS; i++) {
        push(&stack, id * 100 + i);
        usleep(10000); 
    }
    return NULL;
}

void* worker_pop(void* arg) {
    (void)arg;
    int val;
    for (int i = 0; i < NUM_OPS; i++) {
        if (pop(&stack, &val)) {
            printf("Popped: %d\n", val);
        }
        usleep(15000);
    }
    return NULL;
}

int main() {
    atomic_init(&stack.head, NULL);

    pthread_t push_threads[NUM_THREADS];
    pthread_t pop_threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) ids[i] = i;

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&push_threads[i], NULL, worker_push, &ids[i]);

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&pop_threads[i], NULL, worker_pop, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(push_threads[i], NULL);
        pthread_join(pop_threads[i], NULL);
    }

    printf("All operations finished.\n");
    return 0;
}
