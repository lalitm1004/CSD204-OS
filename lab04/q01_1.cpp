/*

    compile and run using:
        make run q01_1
    clean using:
        make clean

    see:
        make help

*/


#include <iostream>
#include <pthread.h>
#include <chrono>
#include <vector>
#include <fstream>

typedef long long ll;

const ll NUM_ELEMENTS = 1000000LL;
const ll VERIFICATION_SUM = NUM_ELEMENTS * (NUM_ELEMENTS + 1) / 2;

ll total_sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct ThreadData {
    ll start;
    ll end;
    ll partial_sum;
};

// simple sequential summing for each thread
void* partial_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    ll partial_sum = 0;

    for (ll i = data->start; i <= data->end; i++) {
        partial_sum += i;
    }

    data->partial_sum = partial_sum;
    return nullptr;
}

ll parallel_sum(int num_threads) {
    // keep track of n threads and their data
    std::vector<pthread_t> threads(num_threads);
    std::vector<ThreadData> thread_data(num_threads);

    // calculate num elements each thread needs to sum
    ll chunk_size = NUM_ELEMENTS / num_threads;

    for (int i = 0; i < num_threads; i++) {
        // start and stop for each thread
        thread_data[i].start = i * chunk_size + 1;
        thread_data[i].end = (i == num_threads - 1) ? NUM_ELEMENTS : (i + 1) * chunk_size;

        // spin up a thread
        pthread_create(&threads[i], nullptr, partial_sum, &thread_data[i]);
    }

    // join each thread and return sum
    ll total_sum = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], nullptr);
        total_sum += thread_data[i].partial_sum;
    }

    return total_sum;
}

int main() {
    std::vector<int> thread_counts = {1, 2, 4, 6, 8, 10, 12, 14, 16};

    // store results in .csv
    std::ofstream results("q01_1.csv");
    results << "Threads,Time(μS)\n";

    for (int num_threads: thread_counts) {
        // spin up n threads
        auto start_time = std::chrono::high_resolution_clock::now();
        ll sum = parallel_sum(num_threads);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        // verify sum
        if (sum != VERIFICATION_SUM) {
            std::cerr << "🚨 Error: Recieved sum of " << sum << " instead of " << VERIFICATION_SUM << " for " << num_threads << " Threads!" << std::endl;
            continue;
        } else {
            std::cout << "✅ " << num_threads << " calculated accurate sum in " << duration.count() << "μS" << std::endl;
            // write to .csv
            results << num_threads << "," << duration.count() << "\n";
        }

    }

    // close and exit
    results.close();
    return EXIT_SUCCESS;
}