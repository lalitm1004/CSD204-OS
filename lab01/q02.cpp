#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

struct CPUInfo {
    int processor_number;
    std::string model_name;
    int cpu_cores;
};

std::vector<CPUInfo> parse_cpu_info() {
    std::ifstream file("/proc/cpuinfo");

    if (!file.is_open()) {
        std::cerr << "unable to read CPU information.\n\n";
    }

    std::vector<CPUInfo> cpus;
    CPUInfo current_cpu = {-1, "", -1};
    std::string line;

    while (std::getline(file, line)) {
        size_t colon_pos = line.find(":");

        // continue if not found
        if (colon_pos == std::string::npos) continue;

        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        // trim strings
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (key == "processor") {
            if (current_cpu.processor_number != -1) {
                cpus.push_back(current_cpu);
            }
            current_cpu = {-1, "", -1};
            current_cpu.processor_number = std::stoi(value);
        }
        else if (key == "model name") {
            current_cpu.model_name = value;
        }
        else if (key == "cpu cores") {
            current_cpu.cpu_cores = std::stoi(value);
        }
    }

    // push final cpu info
    if (current_cpu.processor_number != -1) {
        cpus.push_back(current_cpu);
    }

    file.close();
    return cpus;
}

void printCPUInfo() {
    std::cout
        << "\tCPU Information\n"
        << std::string(30, '-') << '\n';

    std::cout << std::right
        << std::setw(15) << "Processor"
        << "| "
        << std::setw(15) << "Cores"
        << "| Model Name\n";

    std::cout << std::string(15, '-')
        << "+-"
        << std::string(15, '-')
        << "+-"
        << std::string(40, '-') << "\n";

    std::vector<CPUInfo> cpus = parse_cpu_info();
    for (const CPUInfo& cpu: cpus) {
        std::cout << std::right
            << std::setw(14) << cpu.processor_number
            << " |"
            << std::setw(15) << cpu.cpu_cores
            << " | "
            << cpu.model_name << '\n';
    }
}

void printKernelVersion() {
    std::ifstream file("/proc/version");

    if (!file.is_open()) {
        std::cerr << "unable to read kernel version.\n\n";
    }

    std::string line;
    std::getline(file, line);
    std::cout
        << "\tKernel Version\n"
        << std::string(30, '-') << '\n'
        << line << "\n\n";

    file.close();
}

void printMemoryInfo() {
    std::ifstream file("/proc/meminfo");

    if (!file.is_open()) {
        std::cerr << "unable to read memory information.\n\n";
    }

    std::cout
        << "\tMemory Information\n"
        << std::string(30, '-') << '\n';

    std::string line;
    int count = 0;
    while (std::getline(file, line) && count < 3) {
        // only read first three lines
        std::cout << line << "\n";
        ++count;
    }
    std::cout << "\n";

    file.close();
}

int main() {
    printKernelVersion();
    printMemoryInfo();
    printCPUInfo();

    return 0;
}
