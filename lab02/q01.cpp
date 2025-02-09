#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
*   Steps to run:
*       g++ q02.cpp -o q02.out
*       ./q02.out
*/

int main() {
    int N;
    std::cout << "enter number of child processes to create [1, 10] > ";
    std::cin >> N;

    if (N < 1 || N > 10) {
        std::cerr << "error: N must be between 1 and 10" << std::endl;
        return EXIT_FAILURE;
    }

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            std::cout << "Hello i am a child process, pid > " << getpid() << std::endl;
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {
            std::cout << "Hello i am a parent process, pid > " << getpid() << std::endl;
        } else {
            std::cerr << "error: fork failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < N; i++) {
        wait(nullptr);
    }
    std::cout << "parent process | All children reaped" << std::endl;

    return EXIT_SUCCESS;
}