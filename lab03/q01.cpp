#include <iomanip>
#include <iostream>
#include <vector>

/*
    Compile using:
        g++ q01.cpp -o q01.out
        ./q01.out
*/ 

struct Process {
    std::string name;
    int burstTime;
    int waitingTime;
    int turnaroundTime;
};

void takeProcessInput(std::vector<Process>& processQueue) {
    for (int i = 0; i < processQueue.size(); i++) {
        std::cout << "\n-----= Process " << i + 1 << " =----------" << std::endl;
        std::cout << "enter name > ";
        std::cin >> processQueue[i].name;
        std::cout << "enter burstTime > ";
        std::cin >> processQueue[i].burstTime;
    }
}

void simulateFCFS(std::vector<Process>& processQueue) {
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
        << std::setw(15) << "Name "
        << "| "
        << std::setw(15) << "Burst Time "
        << "| "
        << std::setw(15) << "Waiting Time "
        << "| Turnaround Time\n";

    std::cout << std::string(15, '-')
        << "+-"
        << std::string(15, '-')
        << "+-"
        << std::string(15, '-')
        << "+-"
        << std::string(15, '-') << std::endl;

    // display individual details
    for (int i = 0; i < processQueue.size(); i++) {
        std::cout << std::right
            << std::setw(14) << processQueue[i].name
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
    simulateFCFS(processQueue);
    displayTable(processQueue);
    displayAverages(processQueue);

    return EXIT_SUCCESS;
}