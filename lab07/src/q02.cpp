#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

int log2_integer(int number) { return static_cast<int>(std::log2(number)); }

int main() {
  std::srand(std::time(0));

  int processSizeBytes, logicalAddressBits, memorySizeBytes, pageSizeBytes;
  std::cout << "process size (bytes) > ";
  std::cin >> processSizeBytes;

  std::cout << "logical address bits > ";
  std::cin >> logicalAddressBits;

  std::cout << "main memory size (bytes) > ";
  std::cin >> memorySizeBytes;

  std::cout << "page size (bytes) > ";
  std::cin >> pageSizeBytes;

  int offsetBits = log2_integer(pageSizeBytes);
  int pageBits = logicalAddressBits - offsetBits;
  int totalLogicalPages = 1 << pageBits;
  int totalFrames = memorySizeBytes / pageSizeBytes;
  int frameBits = log2_integer(totalFrames);
  int physicalAddressBits = frameBits + offsetBits;
  int totalProcessPages = processSizeBytes / pageSizeBytes;

  std::vector<int> pageTable(totalProcessPages);
  for (int i = 0; i < totalProcessPages; ++i) {
    pageTable[i] = std::rand() % totalFrames;
  }

  std::cout << "\n-----Calculations-----\n";
  std::cout << "logical address bits > " << logicalAddressBits << "\n";
  std::cout << "page number bits > " << pageBits << "\n";
  std::cout << "page offset bits > " << offsetBits << "\n";
  std::cout << "physical address bits > " << physicalAddressBits << "\n";
  std::cout << "frame number bits > " << frameBits << "\n";
  std::cout << "frame offset bits > " << offsetBits << "\n";
  std::cout << "total pages in logical address space > " << totalLogicalPages
            << "\n";
  std::cout << "total frames in main memory > " << totalFrames << "\n";

  int inputLogicalAddress;
  std::cout << "\nenter a logical address to translate (0 to "
            << (processSizeBytes - 1) << "): ";
  std::cin >> inputLogicalAddress;

  int pageNumber = inputLogicalAddress / pageSizeBytes;
  int offset = inputLogicalAddress % pageSizeBytes;

  if (pageNumber >= totalProcessPages) {
    std::cout << "invalid logical address\n";
  } else {
    int frameNumber = pageTable[pageNumber];
    int physicalAddress = frameNumber * pageSizeBytes + offset;
    std::cout << "frame number > " << frameNumber << "\n";
    std::cout << "physical address > " << physicalAddress << "\n";
  }

  return 0;
}
