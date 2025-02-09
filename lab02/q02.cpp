#include <iostream>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>

/*
*   Steps to run:
*       g++ q02.cpp -o q02.out
*       ./q02.out
*
*   Monitor using:
*       watch -n 1 "ps -el | grep -E 'PID|Z|q02'"
*/

void bubbleSort(std::vector<int>& arr) {
    int n = arr.size();
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

void selectionSort(std::vector<int>& arr) {
    int n = arr.size();
    for (size_t i = 0; i < n - 1; i++) {
        size_t min_idx = i;
        for (size_t j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        std::swap(arr[min_idx], arr[i]);
    }
}

void printArray(const std::vector<int>& arr) {
    std::cout << "[ ";
    for (int num: arr)
        std::cout << num << ", ";
    std::cout << "]" << std::endl;
}

void createZombie() {
    pid_t zombie_pid = fork();

    if (zombie_pid == 0) {
        std::cout << "Child (pid " << getpid() << ")" << std::endl;
        exit(EXIT_SUCCESS);

    } else if (zombie_pid > 0) {
        std::cout << "Parent (pid " << getpid() << ") sleeping for 30 seconds" << std::endl;
        sleep(30);
        wait(nullptr);
        std::cout << "Zombie process collected" << std::endl;

    } else {
        std::cerr << "error: fork failed" << std::endl;
        exit(EXIT_FAILURE);

    }
}

void createOrphan() {
    pid_t orphan_pid = fork();

    if (orphan_pid == 0) {
        std::cout << "Child (pid " << getpid() << ")" << std::endl;
        sleep(30);
        std::cout << "Child (pid " << getpid << ") adopted by init process" << std::endl;
        exit(EXIT_SUCCESS);

    } else if (orphan_pid > 0) {
        std::cout << "Parent (pid " << getpid() << ") exiting immediately" << std::endl;
        exit(EXIT_SUCCESS);

    } else {
        std::cerr << "error: fork failed" << std::endl;
        exit(EXIT_FAILURE);

    }
}

int main() {
    int numCount;
    std::cout << "enter number of integers in array > ";
    std::cin >> numCount;

    std::vector<int> arr(numCount);
    std::cout << "enter " << numCount << " integers (separated by space) > ";
    for (int i = 0; i < numCount; i++) {
        std::cin >> arr[i];
    }

    std::cout << "Before sorting ";
    printArray(arr);

    // create copy for child process
    std::vector<int> arr_cpy = arr;

    pid_t pid = fork();

    if (pid == 0) {
        // child
        std::cout << "Child (pid " << getpid() << ") using selection sort" << std::endl;
        selectionSort(arr_cpy);
        std::cout << "After sorting by child process ";
        printArray(arr_cpy);

    } else if (pid > 0) {
        // parent
        std::cout << "Parent (pid " << getpid() << ") using bubble sort" << std::endl;
        selectionSort(arr);
        std::cout << "After sorting by parent process ";
        printArray(arr);

        wait(nullptr);
        std::cout << "Child process completed" << std::endl;

        // fork to display the two processes independently
        pid_t demo_pid = fork();

        if (demo_pid == 0) {
            // demo zombie in child
            std::cout << "\nDemonstrating Zombie Process" << std::endl;
            createZombie();
        } else if (demo_pid > 0) {
            // demo orphan in parent
            wait(nullptr);

            std::cout << "\nDemonstrating Orphan Process" << std::endl;
            createOrphan();
        } else {
            std::cerr << "error: fork failed" << std::endl;
            exit(EXIT_FAILURE);
        }

    } else {
        std::cerr << "error: fork failed" << std::endl;
        exit(EXIT_FAILURE);

    }

    return EXIT_SUCCESS;
}