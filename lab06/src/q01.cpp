#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <random>
#include <unistd.h>

/*
  Compile and run using:
    make run q01
  or
    g++ q01.cpp -o q01 -pthread
    ./q01
*/

int n, k;
double lambda1, lambda2;

std::atomic_flag lockTAS = ATOMIC_FLAG_INIT; // TAS lock
std::atomic<int> lockCAS(0);                 // CAS lock
std::atomic<int> ticket(0);                  // Bounded CAS ticket
std::atomic<int> turn(0);                    // Bounded CAS turn

pthread_mutex_t log_mutex;

struct ThreadParams {
  int thread_id;
  std::ofstream *log;
};

void exp_delay(double lambda) {
  static thread_local std::random_device rd;
  static thread_local std::mt19937 gen(rd());
  std::exponential_distribution<> dist(1.0 / lambda);
  usleep(static_cast<useconds_t>(dist(gen) * 1000));
}

void log_event(std::ofstream &log, int thread_id, int iter,
               const std::string &event) {
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) %
            1000;

  pthread_mutex_lock(&log_mutex);
  log << "TID " << std::setw(2) << std::setfill(' ') << thread_id << " | "
      << "Iter " << std::setw(2) << std::setfill(' ') << iter << " | "
      << std::setw(10) << event << " | "
      << std::put_time(localtime(&time), "%H:%M:%S") << "." << ms.count()
      << std::endl;
  pthread_mutex_unlock(&log_mutex);
}

void *tas(void *arg) {
  ThreadParams *args = (ThreadParams *)arg;
  std::ofstream &log = *(args->log);

  for (int i = 1; i <= k; i++) {
    log_event(log, args->thread_id, i, "Requested");

    while (lockTAS.test_and_set(std::memory_order_acquire))
      ; // Enter Critial Section
    log_event(log, args->thread_id, i, "Entered");

    exp_delay(lambda1); // simulate work inside CS

    lockTAS.clear(std::memory_order_release);
    log_event(log, args->thread_id, i, "Exited");

    exp(lambda2); // simulate work outside CS
  }

  return nullptr;
}

void *cas(void *arg) {
  ThreadParams *args = (ThreadParams *)arg;
  std::ofstream &log = *(args->log);

  for (int i = 1; i <= k; i++) {
    log_event(log, args->thread_id, i, "Requested");

    int expected = 0;
    while (
        !lockCAS.compare_exchange_weak(expected, 1, std::memory_order_acquire))
      ; // Enter Critial Section
    log_event(log, args->thread_id, i, "Entered");

    exp_delay(lambda1); // simulate work inside CS

    lockCAS.store(0, std::memory_order_release);
    log_event(log, args->thread_id, i, "Exited");

    exp(lambda2); // simulate work outside CS
  }

  return nullptr;
}

void *bounded_cas(void *arg) {
  ThreadParams *params = static_cast<ThreadParams *>(arg);
  std::ofstream &log = *(params->log);

  for (int i = 1; i <= k; ++i) {
    log_event(log, params->thread_id, i, "Requested");

    int my_ticket = ticket.fetch_add(1, std::memory_order_acquire);
    while (turn.load(std::memory_order_acquire) != my_ticket)
      ; // Enter Critical Section
    log_event(log, params->thread_id, i, "Entered");

    exp_delay(lambda1);

    turn.store(my_ticket + 1, std::memory_order_release);
    log_event(log, params->thread_id, i, "Exited");

    exp_delay(lambda2);
  }
  return nullptr;
}

void run_test(void *(*algo)(void *), const std::string &filename) {
  pthread_t threads[n];
  ThreadParams args[n];
  std::ofstream log(filename);

  for (int i = 0; i < n; i++) {
    args[i] = {i + 1, &log};
    pthread_create(&threads[i], nullptr, algo, &args[i]);
  }

  for (int i = 0; i < n; i++) {
    pthread_join(threads[i], nullptr);
  }

  log.close();
}

int main() {
  std::ifstream input("input.txt");
  if (!input) {
    std::cerr << "Error: Unable to open input file." << std::endl;
    return EXIT_FAILURE;
  }

  pthread_mutex_init(&log_mutex, nullptr);
  input >> n >> k >> lambda1 >> lambda2;
  input.close();

  std::cout << "Running TAS..." << std::endl;
  run_test(tas, "tas.txt");

  std::cout << "Running CAS..." << std::endl;
  run_test(tas, "cas.txt");

  std::cout << "Running CAS Bounded..." << std::endl;
  run_test(tas, "cas_bounded.txt");

  pthread_mutex_destroy(&log_mutex);
  std::cout << "Simulation completed. Logs written to output files."
            << std::endl;
  return EXIT_SUCCESS;
}