#include <iostream>
#include <vector>

const int kNumProcesses = 5;
const int kNumResourceTypes = 3;

bool IsSystemSafe(std::vector<int> &available,
                  std::vector<std::vector<int>> &maxMatrix,
                  std::vector<std::vector<int>> &allocationMatrix,
                  std::vector<std::vector<int>> &needMatrix,
                  std::vector<int> &safeSequence) {
  std::vector<int> work = available;
  std::vector<bool> finished(kNumProcesses, false);
  safeSequence.clear();

  int finishedCount = 0;
  while (finishedCount < kNumProcesses) {
    bool progressMade = false;
    for (int process = 0; process < kNumProcesses; ++process) {
      if (!finished[process]) {
        bool canAllocate = true;
        for (int res = 0; res < kNumResourceTypes; ++res) {
          if (needMatrix[process][res] > work[res]) {
            canAllocate = false;
            break;
          }
        }
        if (canAllocate) {
          for (int res = 0; res < kNumResourceTypes; ++res)
            work[res] += allocationMatrix[process][res];
          finished[process] = true;
          safeSequence.push_back(process);
          finishedCount++;
          progressMade = true;
        }
      }
    }
    if (!progressMade) {
      return false;
    }
  }
  return true;
}

bool RequestResources(int processId, std::vector<int> request,
                      std::vector<int> &available,
                      std::vector<std::vector<int>> &maxMatrix,
                      std::vector<std::vector<int>> &allocationMatrix,
                      std::vector<std::vector<int>> &needMatrix) {

  for (int res = 0; res < kNumResourceTypes; ++res) {
    if (request[res] > needMatrix[processId][res]) {
      std::cout << "Process has exceeded its maximum claim.\n";
      return false;
    }
  }

  for (int res = 0; res < kNumResourceTypes; ++res) {
    if (request[res] > available[res]) {
      std::cout << "Process must wait. Not enough resources available.\n";
      return false;
    }
  }

  for (int res = 0; res < kNumResourceTypes; ++res) {
    available[res] -= request[res];
    allocationMatrix[processId][res] += request[res];
    needMatrix[processId][res] -= request[res];
  }

  std::vector<int> safeSequence;
  if (IsSystemSafe(available, maxMatrix, allocationMatrix, needMatrix,
                   safeSequence)) {
    std::cout << "Request granted. System is in a safe state.\n";
    return true;
  } else {
    for (int res = 0; res < kNumResourceTypes; ++res) {
      available[res] += request[res];
      allocationMatrix[processId][res] -= request[res];
      needMatrix[processId][res] += request[res];
    }
    std::cout << "Request denied. System would enter an unsafe state.\n";
    return false;
  }
}

int main() {
  std::vector<std::vector<int>> allocationMatrix(
      kNumProcesses, std::vector<int>(kNumResourceTypes));
  std::vector<std::vector<int>> maxMatrix(kNumProcesses,
                                          std::vector<int>(kNumResourceTypes));
  std::vector<std::vector<int>> needMatrix(kNumProcesses,
                                           std::vector<int>(kNumResourceTypes));
  std::vector<int> totalResources(kNumResourceTypes);
  std::vector<int> availableResources(kNumResourceTypes);

  std::cout << "Enter Allocation Matrix:\n";
  for (int i = 0; i < kNumProcesses; ++i)
    for (int j = 0; j < kNumResourceTypes; ++j)
      std::cin >> allocationMatrix[i][j];

  std::cout << "Enter Max Matrix:\n";
  for (int i = 0; i < kNumProcesses; ++i)
    for (int j = 0; j < kNumResourceTypes; ++j)
      std::cin >> maxMatrix[i][j];

  std::cout << "Enter Total Resources:\n";
  for (int j = 0; j < kNumResourceTypes; ++j)
    std::cin >> totalResources[j];

  for (int j = 0; j < kNumResourceTypes; ++j) {
    int allocatedSum = 0;
    for (int i = 0; i < kNumProcesses; ++i)
      allocatedSum += allocationMatrix[i][j];
    availableResources[j] = totalResources[j] - allocatedSum;
  }

  for (int i = 0; i < kNumProcesses; ++i)
    for (int j = 0; j < kNumResourceTypes; ++j)
      needMatrix[i][j] = maxMatrix[i][j] - allocationMatrix[i][j];

  std::vector<int> safeSequence;
  if (IsSystemSafe(availableResources, maxMatrix, allocationMatrix, needMatrix,
                   safeSequence)) {
    std::cout << "Safe sequence is: ";
    for (std::size_t i = 0; i < safeSequence.size(); ++i) {
      std::cout << "P" << safeSequence[i] + 1;
      if (i != safeSequence.size() - 1)
        std::cout << " -> ";
    }
    std::cout << '\n';
  } else {
    std::cout << "No safe sequence found.\n";
  }

  return 0;
}
