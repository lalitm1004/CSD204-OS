/*
    compile and run using:
        make run q01_2
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

// use uint128 for large numbers and accurate benchmarking
using u128 = __uint128_t;

const u128 NUM_ELEMENTS = (u128)1e15;
const u128 VERIFICATION_SUM = NUM_ELEMENTS * (NUM_ELEMENTS + 1) / 2;
const u128 BLOCK_SIZE = 1000000;

struct ThreadData {
    u128 start;
    u128 end;
    u128 partial_sum;
};

void print_u128(u128 n) {
    if (n == 0) {
        std::cout << "0";
        return;
    }

    std::string str;
    while (n > 0) {
        str = char('0' + n % 10) + str;
        n /= 10;
    }
    std::cout << str;
}

void* partial_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    u128 partial_sum = 0;

    // use n * (start + end) / 2 to make it slightly more optimized
    for (u128 block_start = data->start; block_start <= data->end; block_start += BLOCK_SIZE) {
        u128 block_end = std::min(block_start + BLOCK_SIZE - 1, data->end);

        u128 n = block_end - block_start + 1;

        u128 block_sum = (n * (block_start + block_end)) / 2;
        partial_sum += block_sum;
    }

    data->partial_sum = partial_sum;
    return nullptr;
}

u128 parallel_sum(int num_threads) {
    std::vector<pthread_t> threads(num_threads);
    std::vector<ThreadData> thread_data(num_threads);

    u128 chunk_size = NUM_ELEMENTS / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].start = i * chunk_size + 1;
        thread_data[i].end = (i == num_threads - 1) ? NUM_ELEMENTS : (i + 1) * chunk_size;

        pthread_create(&threads[i], nullptr, partial_sum, &thread_data[i]);
    }

    u128 total_sum = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], nullptr);
        total_sum += thread_data[i].partial_sum;
    }

    return total_sum;
}

int main() {
    std::vector<int> thread_counts = {1, 2, 4, 6, 8, 10, 12, 14, 16};
    std::ofstream results("q01_2.csv");
    results << "Threads,Time(mS)\n";

    for (int num_threads: thread_counts) {
        auto start_time = std::chrono::high_resolution_clock::now();
        u128 sum = parallel_sum(num_threads);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        // verify sum
        if (sum != VERIFICATION_SUM) {
            std::cerr << "🚨 Error: Received sum of "; print_u128(sum);
            std::cerr << " instead of "; print_u128(VERIFICATION_SUM);
            std::cerr << " for " << num_threads << " Threads!" << std::endl;
            continue;
        } else {
            std::cout << "✅ " << num_threads << " Threads calculated accurate sum in " << duration.count() << "mS" << std::endl;
        }

        results << num_threads << "," << duration.count() << "\n";
    }

    results.close();
    return EXIT_SUCCESS;
}