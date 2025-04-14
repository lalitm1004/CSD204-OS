#align(center)[
  = CSD204 - Operating System
  = Lalit Maurya, 2310110164
]

= Question 01

#align(center)[
  #image("q1.png")
]

This program implements the Banker's Algorithm to check system safety and handle resource requests by processes.

== Features:
- Accepts input for Allocation, Max, and Total system resources.
- Computes Available and Need matrices.
- Determines whether the system is in a safe state by calculating a safe sequence.
- Allows runtime resource requests by processes and checks if granting the request keeps the system in a safe state.

= Question 02

#align(center)[
  #image("q2.png")
]

This program simulates logical to physical address translation using paging.

== Features:
- Takes inputs for process size, logical address bits, main memory size, and page size.
- Calculates:
  - Page number and offset bits
  - Frame number bits and physical address bits
- Randomly generates a page table.
- Accepts a logical address and translates it to a physical address using the page table.