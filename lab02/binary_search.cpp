#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstdlib>

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
            std::cerr << "error: number out of range" << token << std::endl;
            exit(1);
        }
    }
    return arr;
}

int binarySearch(const std::vector<int>& arr, int target) {
    int low = 0;
    int high = arr.size() - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2; // avoid overflow
        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: binary_search <sorted_array_string>" << std::endl;
        return 1;
    }

    std::string sortedArrayStr = argv[1];
    std::vector<int> sortedArray = stringToArray(sortedArrayStr);

    std::cout << "\n----------= Inside binary_search.cpp =----------\n";

    int target;
    std::cout << "enter search target > ";
    std::cin >> target;

    int index = binarySearch(sortedArray, target);

    if (index != -1) {
        std::cout << "element " << target << " found at index " << index << std::endl;
    } else {
        std::cout << "element " << target << " not found" << std::endl;
    }

    return 0;
}