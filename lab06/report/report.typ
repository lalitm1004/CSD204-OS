#align(center)[
  = CSD204 - OS - Lab06
  = Lalit Maurya, 2310110164
]

= Analysis of different Mutual Exclusion Algorithms in C++

For this assignment, I wrote a script `q01.cpp` that reads from an input file and produces three output files for the following algorithms:
- TAS
- CAS
- Bounded CAS

The program accepts
- `n`: Number of threads
- `k`: Number of iterations for critical section
- $lambda_1$: Exponential Distribution Argument 1
- $lambda_2$: Exponential Distribution Argument 2

It then creates multiple locks and a log file for each algorithm. By varying the `input.txt` file and rerunning, we can benchmark the different algorithms.

Since the thread count had to be varied from 10 to 50 and 5 reruns were necessary at each go, I opted to create a python script to handle rerunning the script for me.

= Outputs

Throughout the tests:
- `k` = 10
- $lambda_1$ = 10
- $lambda_2$ = 10

#image("mutex_performance_comparison.png", width: 80%)

#let results = csv("mutex_performance_results.csv")

== Waiting Times
#table(
  columns:7,
  [*Threads*],
  [*TAS Avg*],
  [*TAS Worst*],
  [*CAS Avg*],
  [*CAS Worst*],
  [*Bounded CAS Avg*],
  [*Bounded CAS Worst*],
  ..results.flatten(),
)

== Performance Comparision
TAS performs best for low thread counts in terms of average waiting time
CAS shows most fair behaviour (smallest worst-case wait time)

== Conclusion
- Where raw performance is critical and thread count is low, TAS is preferable
- Where fairness is critical, Bounded CAS is preferable