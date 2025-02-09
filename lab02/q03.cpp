#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <sstream>
#include <cstdlib>

/*
*   Steps to run:
*       g++ binary_search.cpp -o binary_search.out
*       g++ q03.cpp -o q03.out
*       ./q03.out
*/

// serialization function
std::string arrayToString(const std::vector<int>& arr) {
    std::stringstream ss;
    for (int i = 0; i < arr.size(); ++i) {
        ss << arr[i];
        if (i < arr.size() - 1) {
            ss << ","; // comma delimiter
        }
    }
    return ss.str();
}

// deserialization function
std::vector<int> stringToArray(const std::string& str) {
    std::vector<int> arr;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, ',')) {
        try {
            arr.push_back(stoi(token));
        } catch (const std::invalid_argument& e) {
            std::cerr << "error: invalid number format " << token << std::endl;
            exit(1);
        } catch (const std::out_of_range& e) {
            std::cerr << "error: number out of range " << token << std::endl;
            exit(1);
        }
    }
    return arr;
}

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

void printArray(const std::vector<int>& arr) {
    std::cout << "[ ";
    for (int num: arr)
        std::cout << num << ", ";
    std::cout << "]" << std::endl;
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

    // create a pipe for inter process communication
    int pipefd[2]; // 0 -> read; 1 -> write;
    if (pipe(pipefd) == -1) {
        std::cerr << "error: pipe creation failed" << std::endl;
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // child
        close(pipefd[1]); // close write end

        // read sortedArrStr from parent and close read end
        char buffer[2048];
        read(pipefd[0], buffer, sizeof(buffer));
        close(pipefd[0]);

        std::cout << "\nchild: received sortedArrStr" << std::endl;

        execlp("sh", "sh", "-c", ("./binary_search.out \"" + std::string(buffer) + "\"").c_str(), nullptr);
        std::cerr << "error: execl failed" << std::endl; // should not reach here if execl succeeds
        return EXIT_FAILURE;

    } else if (pid > 0) {
        // parent
        close(pipefd[0]); // close read end

        bubbleSort(arr);
        std::string sortedArrayStr = arrayToString(arr);

        std::cout << "\nparent: sorted array: ";
        printArray(stringToArray(sortedArrayStr));

        // write sortedArrStr to child and close write end
        write(pipefd[1], sortedArrayStr.c_str(), sortedArrayStr.length() + 1);
        close(pipefd[1]);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "\n----------= Inside q03.cpp =----------";
            std::cout << "\nchild process exited with status " << WEXITSTATUS(status) << std::endl;
        }

    } else {
        std::cerr << "error: fork failed" << std::endl;
        return EXIT_FAILURE;

    }

    return EXIT_SUCCESS;
}
