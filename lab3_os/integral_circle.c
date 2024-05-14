#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>

// Supplier-Consumer Model
int counter = 10;
pthread_mutex_t supply_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t supply_condition = PTHREAD_COND_INITIALIZER;

void *supplier_function(void *arg) {
    while (1) {
        pthread_mutex_lock(&supply_mutex);
        pthread_cond_wait(&supply_condition, &supply_mutex);
        
        pthread_mutex_unlock(&supply_mutex);
        while (counter < 10) {
            counter++;
            printf("Supplied: %d\n", counter);
            sleep(1);
        }
        pthread_cond_signal(&supply_condition);
    }
    pthread_exit(NULL);
}

void *consumer_function(void *arg) {
    while (1) {
        
        while (counter > 0) {
            counter--;
            printf("Consumed: %d\n", counter);
            sleep(1);
        }
        pthread_mutex_lock(&supply_mutex);
        pthread_cond_signal(&supply_condition);
        pthread_cond_wait(&supply_condition, &supply_mutex);
        pthread_mutex_unlock(&supply_mutex);
    }
    pthread_exit(NULL);
}

int main() {




    pthread_t supplier_thread, consumer_thread;
    pthread_create(&supplier_thread, NULL, supplier_function, NULL);
    pthread_create(&consumer_thread, NULL, consumer_function, NULL);

    pthread_join(supplier_thread, NULL);
    pthread_join(consumer_thread, NULL);

    return 0;
}
