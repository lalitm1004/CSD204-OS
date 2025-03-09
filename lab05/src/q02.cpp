#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <unistd.h>

/*
  run using:
    make run q02
*/

#define NUM_USERS 2

std::queue<int> print_queue;
sem_t job_count;
pthread_mutex_t mutex;

void display_queue() {
  std::queue<int> temp(print_queue);

  std::cout << "[ ";
  while (!temp.empty()) {
    std::cout << temp.front() << ", ";
    temp.pop();
  }
  std::cout << " ]" << std::endl;
}

void *user(void *arg) {
  int id = *(int *)arg;
  while (true) {
    int job_id = rand() % 100;
    pthread_mutex_lock(&mutex); // get exclusive access to spooler queue

    print_queue.push(job_id);
    std::cout << "U" << id << " +" << job_id << ' ';
    display_queue();
    sem_post(&job_count); // signal creation of print job

    pthread_mutex_unlock(&mutex);
    sleep(rand() % 4 + 1); // simulate random production delay
  }

  return nullptr;
}

void *printer(void *arg) {
  (void)arg; // mark unused to suppress compile warning

  while (true) {
    sem_wait(&job_count);       // decrement num of print jobs
    pthread_mutex_lock(&mutex); // get exclusive access to spooler queue

    if (!print_queue.empty()) {
      int job_id = print_queue.front();
      print_queue.pop();
      std::cout << "Printer" << " -" << job_id << ' ';
      display_queue();
    }

    pthread_mutex_unlock(&mutex);
    sleep(rand() % 4 + 3); // simluate random printing delay that is greater
                           // than production delay
  }

  return nullptr;
}

int main() {
  pthread_t users[NUM_USERS], printer_thread;
  int user_ids[NUM_USERS];

  sem_init(&job_count, 0, 0);
  pthread_mutex_init(&mutex, nullptr);

  for (int i = 0; i < NUM_USERS; i++) {
    user_ids[i] = i + 1;
    pthread_create(&users[i], nullptr, user, &user_ids[i]);
  }

  pthread_create(&printer_thread, nullptr, printer, nullptr);

  for (int i = 0; i < NUM_USERS; i++) {
    pthread_join(users[i], nullptr);
  }
  pthread_join(printer_thread, nullptr);

  sem_destroy(&job_count);
  pthread_mutex_destroy(&mutex);

  return EXIT_SUCCESS;
}