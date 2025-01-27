#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <dirent.h>
#include <pwd.h>


DIR* getProcDir() {
    DIR* procDir = opendir("/proc");
    if (!procDir) {
        std::cerr << "error > cannot open /proc dir.\n";
        std::exit(EXIT_FAILURE);
    }

    return procDir;
}

void printHeader() {
    std::cout << std::right
        << std::setw(10) << "PID "
        << "| "
        << std::setw(20) << "Name "
        << "| "
        << std::setw(20) << "Status "
        << "| User ID\n";

    std::cout << std::string(10, '-')
        << "+-"
        << std::string(20, '-')
        << "+-"
        << std::string(20, '-')
        << "+-"
        << "---------\n";
}

std::string readValue(const std::string& line, const std::string& key) {
    if (line.find(key) == 0) {
        return line.substr(key.size() + 1);
    }
    return "";
}

void extractProcessDetails(
    const std::string pid,
    std::string& name,
    std::string& status,
    std::string& uid
) {
    std::string path = "/proc/" + pid + "/status";
    std::ifstream statusFile(path);

    // skip if cannot open
    if (!statusFile.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(statusFile, line)) {
        std::string value;
        if ((value = readValue(line, "Name:")) != "") {
            name = value;
        } else if ((value = readValue(line, "State:")) != "") {
            status = value;
        } else if ((value = readValue(line, "Uid:")) != "") {
            // capture first uid
            uid = value.substr(0, value.find('\t'));
            break;
        }
    }

    statusFile.close();
}

std::string getUsernameFromUid(const std::string& uid) {
    struct passwd* pw = getpwuid(std::stoi(uid));
    return pw ? pw -> pw_name : "unknown";
}

void printProcess(std::string pid) {
    std::string name, status, uid;
    extractProcessDetails(pid, name, status, uid);

    if (!name.empty() && !status.empty() && !uid.empty()) {
        std::string username = getUsernameFromUid(uid);

        std::cout << std::right
            << std::setw(9) << pid
            << " |"
            << std::setw(20) << name
            << " |"
            << std::setw(20) << status
            << " | "
            << username << "\n";
    }
}

void listProcesses() {
    DIR* procDir = getProcDir();

    printHeader();

    struct dirent* entry;
    while ((entry = readdir(procDir)) != nullptr) {
        // skip non-numeric directories
        if (!std::isdigit(entry -> d_name[0])) {
            continue;
        }

        std::string pid = entry -> d_name;
        printProcess(pid);
    }
}

int main() {
    listProcesses();
    return 0;
}