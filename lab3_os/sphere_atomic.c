#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdint.h>
#include <stdatomic.h>

#define NUM_THREADS 5
#define PI 3.14159265359

// Глобальні лічильники
_Atomic int64_t global_counter = 0;
_Atomic int64_t global_success = 0;


// Радіус сфери
const int RADIUS = 1;

struct argument {
    float r;
    int local_counter;
    int local_success;
};

// Функція потоку
void *thread_function(void *arg) {
    struct argument *thread_arg = (struct argument *)arg;
    int64_t local_counter = 0;
    int64_t local_success = 0;
    

    while (atomic_fetch_add(&global_counter, 1) < 1000000) {
        
        // Генерування випадкових координат
        double x = (double)rand() / RAND_MAX * thread_arg->r * 2 - thread_arg->r;
        double y = (double)rand() / RAND_MAX * thread_arg->r * 2 - thread_arg->r;
        double z = (double)rand() / RAND_MAX * thread_arg->r * 2 - thread_arg->r;

        // Перевірка, чи точка знаходиться всередині сфери
        if (x * x + y * y + z * z <= thread_arg->r * thread_arg->r) {
            local_success++;
            atomic_fetch_add(&global_success, 1);
        }
        local_counter++;
        

        
    }

    thread_arg->local_counter = local_counter;
    thread_arg->local_success = local_success;

    // Виведення результатів локального лічильника
    printf("Потік %ld зробив %ld спроб, вдалих - %ld\n", (long)arg, local_counter, local_success);

    pthread_exit(NULL);
}


int main() {
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;
    const int RADIUS = 1;
    srand(time(NULL));

    // Створення потоків
    struct argument args[NUM_THREADS];
    for (t = 0; t < NUM_THREADS; t++) {
        args[t].r = RADIUS;
        rc = pthread_create(&threads[t], NULL, thread_function, &args[t]);
        if (rc) {
            printf("Помилка: pthread_create() повернув код %d\n", rc);
            exit(-1);
        }
    }

    // Очікування завершення потоків
    for (t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // Обчислення суми локальних лічильників
    int64_t sum_of_local_counter = 0;
    int64_t sum_of_local_success = 0;

    for (t = 0; t < NUM_THREADS; t++) {
        sum_of_local_success += args[t].local_success;
        sum_of_local_counter += args[t].local_counter;
    }

    // Виведення глобальних лічильників та обчислення об'єму кулі
    printf("Глобальні лічильники. Загальний: %ld Вдалі спроби: %ld\n", atomic_load(&global_counter), atomic_load(&global_success));
    printf("Сума локальних лічильників: %ld Вдалі спроби: %ld\n", sum_of_local_counter, sum_of_local_success);
    double volume = 4.0 / 3.0 * PI * RADIUS * RADIUS * RADIUS;
    double global_counted_volume = (double)atomic_load(&global_success) / atomic_load(&global_counter) * 8 * RADIUS * RADIUS * RADIUS;
    double local_counted_volume = (double)atomic_load(&sum_of_local_success) / sum_of_local_counter * 8 * RADIUS * RADIUS * RADIUS;
    printf("Точний об'єм: %lf\n", volume);
    printf("Об'єм розрахований через глобальний лічильник: %lf\n", global_counted_volume);
    printf("Об'єм розрахований через локальний лічильник: %lf\n", local_counted_volume);

    return 0;
}
