#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdint.h>

#define NUM_THREADS 5
#define PI 3.14159265359

// Глобальні лічильники та мютекси
int64_t global_counter = 0;
int64_t global_success = 0;
int64_t sum_of_local_counter = 0;
int64_t sum_of_local_success = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t success_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sum_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sum_success_mutex = PTHREAD_MUTEX_INITIALIZER;

// Радіус сфери
const int RADIUS = 1;

// Функція потоку
void *thread_function(void *arg) {
    int64_t local_counter = 0;
    int64_t local_success = 0;

    while (1) {
        pthread_mutex_lock(&counter_mutex);
        
        if (global_counter >= 1000000) {
            pthread_mutex_unlock(&counter_mutex);
            break;
        }
        global_counter++;
        pthread_mutex_unlock(&counter_mutex);

        // Генерування випадкових координат
        double x = (double)rand() / RAND_MAX * RADIUS * 2 - RADIUS;
        double y = (double)rand() / RAND_MAX * RADIUS * 2 - RADIUS;
        double z = (double)rand() / RAND_MAX * RADIUS * 2 - RADIUS;

        // Перевірка, чи точка знаходиться всередині сфери
        if (x*x + y*y + z*z <= RADIUS*RADIUS) {
            local_success++;
        }
        local_counter++;

        
    }

    pthread_mutex_lock(&sum_counter_mutex);
    sum_of_local_counter += local_counter;
    sum_of_local_success += local_success;
    global_success += local_success;
    pthread_mutex_unlock(&sum_counter_mutex);

    // Виведення результатів локального лічильника
    printf("Потік %ld зробив %ld спроб, вдалих - %ld\n", (long)arg, local_counter, local_success);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;

    srand(time(NULL));

    // Створення потоків
    for (t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, thread_function, (void *)t);
        if (rc) {
            printf("Помилка: pthread_create() повернув код %d\n", rc);
            exit(-1);
        }
    }

    // Очікування завершення потоків
    for (t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // Виведення глобальних лічильників та обчислення об'єму кулі
    printf("Глобальні лічильники. Загальний: %ld Вдалі спроби: %ld\n", global_counter, global_success);
    printf("Сума локальних лічильників. Загальний: %ld Вдалі спроби: %ld\n", sum_of_local_counter, sum_of_local_success);
    double volume = 4.0/3.0 * PI * RADIUS * RADIUS * RADIUS;
    double global_counted_volume =  (double)global_success / global_counter * 8 * RADIUS * RADIUS * RADIUS;
    double local_counted_volume =  (double)sum_of_local_success / sum_of_local_counter * 8 * RADIUS * RADIUS * RADIUS;
    printf("Точний об'єм: %lf\n", volume);
    printf("Об'єм розрахований через глобальний лічильник: %lf\n", global_counted_volume);
    printf("Об'єм розрахований через локальний лічильник: %lf\n", local_counted_volume);

    return 0;
}
