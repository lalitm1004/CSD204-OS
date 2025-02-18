#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>

/*
    Compile using:
        g++ q04.cpp -o q04.out
        ./q04.out
*/ 

struct Process {
    int pid;
    int burstTime;
    int remainingTime;
    int waitingTime;
    int turnaroundTime;
};

void takeProcessInput(std::vector<Process>& processQueue) {
    for (int i = 0; i < processQueue.size(); i++) {
        std::cout << "\n-----= Process " << i + 1 << " =----------" << std::endl;
        processQueue[i].pid = 100 + i;
        std::cout << "enter burst time > ";
        std::cin >> processQueue[i].burstTime;
        processQueue[i].remainingTime = processQueue[i].burstTime;
    }
}

void displayStats(std::vector<Process>& processQueue) {
    // table header
    std::cout << '\n' << std::right
        << std::setw(5) << "PID "
        << "| "
        << std::setw(15) << "Burst Time "
        << "| "
        << std::setw(15) << "Waiting Time "
        << "| Turnaround Time\n";

    std::cout << std::string(5, '-')
        << "+-"
        << std::string(15, '-')
        << "+-"
        << std::string(15, '-')
        << "+-"
        << std::string(15, '-') << std::endl;
    
    int N = processQueue.size();

    // display individual details
    for (int i = 0; i < N; i++) {
        std::cout << std::right
            << std::setw(4) << processQueue[i].pid
            << " |"
            << std::setw(15) << processQueue[i].burstTime
            << " |"
            << std::setw(15) << processQueue[i].waitingTime
            << " |"
            << std::setw(15) << processQueue[i].turnaroundTime << std::endl;
    }

    std::cout << '\n';

    // calculate averages
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    for (int i = 0; i < N; i++) {
        totalWaitingTime += processQueue[i].waitingTime;
        totalTurnaroundTime += processQueue[i].turnaroundTime;
    }
    
    double avgWaitingTime =  static_cast<double>(totalWaitingTime) / N;
    double avgTurnaroundTime = static_cast<double>(totalTurnaroundTime) / N;

    // display averages
    std::cout
        << std::fixed << std::setprecision(2)
        << "Average Waiting Time > " << avgWaitingTime
        << std::endl;

    std::cout
        << std::fixed << std::setprecision(2)
        << "Average Turnaround Time > " << avgTurnaroundTime
        << std::endl;
}

void simulateRR(std::vector<Process>& processQueue, int timeSlice) {
    int N = processQueue.size();

    int currentTime = 0;
    int numCompleted = 0;

    int currentIndex = 0;
    while (numCompleted != N) {
        if (processQueue[currentIndex].remainingTime > 0) {

            // execute current process for timeSlice or remaining time, whichever is smaller
            int executionTime = std::min(timeSlice, processQueue[currentIndex].remainingTime);
            currentTime += executionTime;
            processQueue[currentIndex].remainingTime -= executionTime;

            // update waiting times for other processes
            for (int i = 0; i < N; i++) {
                if (i != currentIndex && processQueue[i].remainingTime > 0) {
                    processQueue[i].waitingTime += executionTime;
                }
            }

            // mark turnaround time if necessary
            if (processQueue[currentIndex].remainingTime <= 0) {
                numCompleted++;
                processQueue[currentIndex].turnaroundTime = currentTime;
            }
        }
        
        // move currentIndex forward, wrapping as necessary to create illusion of circular queue
        currentIndex += 1;
        currentIndex = currentIndex % N;
    }

    displayStats(processQueue);
}

int main() {
    int N, timeSlice;
    std::cout << "enter time quantum > ";
    std::cin >> timeSlice;
    std::cout << "enter num processes > ";
    std::cin >> N;

    std::vector<Process> processes(N);

    takeProcessInput(processes);
    simulateRR(processes, timeSlice);

    return EXIT_SUCCESS;
}