/*

    compile and run using:
        make run q03
    clean using:
        make clean

    see:
        make help

*/


#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <iomanip>

const int NUM_THREADS = 2;
const int NUM_ITERATIONS = 100000;
int INCREMENT_VALUE = 10;
int EXPECTED = NUM_ITERATIONS * INCREMENT_VALUE * NUM_THREADS;

int counter_without_lock = 0;
int counter_with_lock = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* increment_without_lock(void *arg) {
    int increment_value = *(int *)arg;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        counter_without_lock += increment_value;
    }

    pthread_exit(NULL);
    return nullptr;
}

void* increment_with_lock(void *arg) {
    int increment_value = *(int *)arg;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        counter_with_lock += increment_value;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
    return nullptr;
}

int main() {
    std::vector<pthread_t> threads_without_lock(NUM_THREADS);
    std::vector<pthread_t> threads_with_lock(NUM_THREADS);

    // spin up threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads_without_lock[i], nullptr, increment_without_lock, &INCREMENT_VALUE);
        pthread_create(&threads_with_lock[i], nullptr, increment_with_lock, &INCREMENT_VALUE);
    }

    // join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads_without_lock[i], nullptr);
        pthread_join(threads_with_lock[i], nullptr);
    }

    // print table
    std::cout << std::right
        << std::setw(15) << "Expected "
        << "| "
        << std::setw(15) << "Without Lock "
        << "| "
        << std::setw(15) << "With Lock" << std::endl;

    std::cout << std::string(15, '-')
        << "+-"
        << std::string(15, '-')
        << "+-"
        << std::string(15, '-') << std::endl;

    std::cout << std::right
        << std::setw(14) << EXPECTED
        << " |"
        << std::setw(15) << counter_without_lock
        << " |"
        << std::setw(15) << counter_with_lock << std::endl;

    return EXIT_SUCCESS;
}