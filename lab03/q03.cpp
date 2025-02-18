#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

/*
    Compile using:
        g++ q03.cpp -o q03.out
        ./q03.out
*/ 

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
};

void takeProcessInput(std::vector<Process>& processQueue) {
    for (int i = 0; i < processQueue.size(); i++) {
        std::cout << "\n-----= Process " << i + 1 << " =----------" << std::endl;
        processQueue[i].pid = 100 + i;
        std::cout << "enter burst time > ";
        std::cin >> processQueue[i].burstTime;
        std::cout << "enter arrival time > ";
        std::cin >> processQueue[i].arrivalTime;
        processQueue[i].remainingTime = processQueue[i].burstTime;
    }
}

void displayStats(std::vector<Process>& processQueue, int totalWaitingTime, int totalTurnaroundTime) {
    // table header
    std::cout << '\n' << std::right
        << std::setw(5) << "PID "
        << "| "
        << std::setw(15) << "Arrival Time "
        << "| "
        << std::setw(15) << "Burst Time "
        << "| "
        << std::setw(15) << "Completion Time "
        << "| "
        << std::setw(15) << "Waiting Time "
        << "| Turnaround Time\n";

    std::cout << std::string(5, '-')
        << "+-"
        << std::string(15, '-')
        << "+-"
        << std::string(15, '-')
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
            << std::setw(15) << processQueue[i].arrivalTime
            << " |"
            << std::setw(15) << processQueue[i].burstTime
            << " |"
            << std::setw(15) << processQueue[i].completionTime
            << " |"
            << std::setw(15) << processQueue[i].waitingTime
            << " |"
            << std::setw(15) << processQueue[i].turnaroundTime << std::endl;
    }

    std::cout << '\n';

    double avgWaitingTime = static_cast<double>(totalWaitingTime) / N;
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

void simulateSRTF(std::vector<Process>& processQueue) {
    int N = processQueue.size();

    int currentTime = 0;
    int numCompleted = 0;
    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;

    while (numCompleted != N) {
        int index = -1;
        int minRemainingTime = std::numeric_limits<int>::max();

        // find process with minimum remaining time
        for (int i = 0; i < N; i++) {
            if (
                processQueue[i].arrivalTime <= currentTime && // process has arrived
                processQueue[i].remainingTime > 0 && // process needs cpu time
                processQueue[i].remainingTime < minRemainingTime // shorted possible time
            ) {
                minRemainingTime = processQueue[i].remainingTime;
                index = i; // select process
            }
        }

        // increment time if no arrivals
        if (index == -1) {
            currentTime++;
            continue;
        }

        // execute selected process for one time unit;
        processQueue[index].remainingTime--;
        currentTime++;

        // if selected process is complete, calculate its times
        if (processQueue[index].remainingTime <= 0) {
            numCompleted++;
            processQueue[index].completionTime = currentTime;
            processQueue[index].turnaroundTime = processQueue[index].completionTime - processQueue[index].arrivalTime;
            processQueue[index].waitingTime = processQueue[index].turnaroundTime - processQueue[index].burstTime;

            totalWaitingTime += processQueue[index].waitingTime;
            totalTurnaroundTime += processQueue[index].turnaroundTime;
        }
    }

    displayStats(processQueue, totalWaitingTime, totalTurnaroundTime);
}

int main() {
    int N;
    std::cout << "enter num processes > ";
    std::cin >> N;

    std::vector<Process> processQueue(N);

    takeProcessInput(processQueue);
    simulateSRTF(processQueue);

    return EXIT_SUCCESS;
}
