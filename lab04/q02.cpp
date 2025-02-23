/*

    compile and run using:
        make run q02
    clean using:
        make clean

    see:
        make help

*/


#include <iostream>
#include <vector>
#include <pthread.h>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <fstream>

const int ARRAY_SIZE = 10000000;
const int MAX_NUM = 10000000;

struct ThreadData {
    std::vector<int>* arr;
    int start;
    int end;
};

// traiditional merge and merger sort function
void merge(std::vector<int>& arr, int left, int mid, int right) {
    std::vector<int> temp(right - left + 1);
    int i = left;
    int j = mid + 1;
    int k = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i <= mid)
        temp[k++] = arr[i++];

    while (j <= right)
        temp[k++] = arr[j++];

    for (i = 0; i < k; i++) {
        arr[left + i] = temp[i];
    }
}

void mergeSort(std::vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// calls traditional mergesort for each thread
void* parallelMergeSort(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int left = data->start;
    int right = data->end;

    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(*(data->arr), left, mid);
        mergeSort(*(data->arr), mid + 1, right);
        merge(*(data->arr), left, mid, right);
    }

    pthread_exit(NULL);
    return nullptr;
}

std::vector<int> generateArr() {
    std::vector<int> arr(ARRAY_SIZE);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, MAX_NUM);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = dis(gen);
    }
    return arr;
}

bool isSorted(const std::vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i - 1]) return false;
    }
    return true;
}

void parallelSort(std::vector<int>& arr, int num_threads) {
    std::vector<pthread_t> threads(num_threads);
    std::vector<ThreadData> thread_data(num_threads);

    int chunk_size = ARRAY_SIZE / num_threads;

    // spin up threads with respective args
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].arr = &arr;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? ARRAY_SIZE - 1 : (i + 1) * chunk_size - 1;

        pthread_create(&threads[i], nullptr, parallelMergeSort, &thread_data[i]);
    }

    // join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], nullptr);
    }

    // final merge of sorted subarrays
    for (int size = chunk_size; size < ARRAY_SIZE; size *= 2) {
        for (int left = 0; left < ARRAY_SIZE - size; left += 2 * size) {
            int mid = left + size - 1;
            int right = std::min(left + 2 * size - 1, ARRAY_SIZE - 1);
            merge(arr, left, mid, right);
        }
    }
}

int main() {
    std::vector<int> arr = generateArr();

    std::vector<int> thread_counts = {1, 2, 4, 6, 8, 10, 12, 14, 16};
    // store results in .csv
    std::ofstream results("q02.csv");
    results << "Threads,Time(mS)\n";

    for (int num_threads: thread_counts) {
        std::vector<int> arr_copy = arr;

        // spin up n threads
        auto start_time = std::chrono::high_resolution_clock::now();
        parallelSort(arr_copy, num_threads);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        // verify
        if (!isSorted(arr_copy)) {
            std::cerr << "🚨 Error: Failure in sorting array for " << num_threads << " Threads!" << std::endl;
            continue;
        } else {
            std::cout << "✅ " << num_threads << " Threads successfully sorted array in "  << duration.count() << "mS" << std::endl;
            // write to .csv
            results << num_threads << "," << duration.count() << "\n";
        }
    }

    results.close();
    return EXIT_SUCCESS;
}