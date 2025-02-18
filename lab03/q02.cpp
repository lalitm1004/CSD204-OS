#include <iomanip>
#include <iostream>
#include <vector>

/*
    Compile using:
        g++ q02.cpp -o q02.out
        ./q02.out
*/ 

struct Process {
    int pid;
    int burstTime;
    int waitingTime;
    int turnaroundTime;
};

void takeProcessInput(std::vector<Process>& processQueue) {
    for (int i = 0; i < processQueue.size(); i++) {
        std::cout << "\n-----= Process " << i + 1 << " =----------" << std::endl;
        processQueue[i].pid = 100 + i;
        std::cout << "enter burst time > ";
        std::cin >> processQueue[i].burstTime;
    }
}

void sortProcessQueue(std::vector<Process>& processQueue) {
    // use bubble sort to sort based on burstTime
    int N = processQueue.size();
    for (int i = 0; i < N - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < N - i - 1; j++) {
            if (processQueue[j].burstTime > processQueue[j + 1].burstTime) {
                std::swap(processQueue[j], processQueue[j + 1]);
                swapped = true;
            }
        }

        if (!swapped) break;
    }
}

void simulateSJF(std::vector<Process>& processQueue) {
    // initalize first process
    processQueue[0].waitingTime = 0;
    processQueue[0].turnaroundTime = processQueue[0].burstTime;

    // calculate other attributes for successive processes
    for (int i = 1; i < processQueue.size(); i++) {
        processQueue[i].waitingTime = processQueue[i - 1].waitingTime + processQueue[i - 1].burstTime;
        processQueue[i].turnaroundTime = processQueue[i].waitingTime + processQueue[i].burstTime;
    }
}

void displayTable(const std::vector<Process>& processQueue) {
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

    // display individual details
    for (int i = 0; i < processQueue.size(); i++) {
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
}

void displayAverages(const std::vector<Process>& processQueue) {
    // calc (im just using slang guys) averages
    int N = processQueue.size();

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

int main() {
    int N;
    std::cout << "enter num processes > ";
    std::cin >> N;

    std::vector<Process> processQueue(N);

    takeProcessInput(processQueue);
    sortProcessQueue(processQueue);
    simulateSJF(processQueue);
    displayTable(processQueue);
    displayAverages(processQueue);

    return EXIT_SUCCESS;
}