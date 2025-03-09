#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <unistd.h>

/*
  run using:
    make run q01
*/

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3

std::queue<int> buffer;
sem_t item_count;
pthread_mutex_t mutex;

void *producer(void *arg) {
  int id = *(int *)arg;
  while (true) {
    int item = rand() % 100;
    pthread_mutex_lock(&mutex); // get exclusive access to buffer

    // only continue if buffer is not empty
    if (buffer.size() < BUFFER_SIZE) {
      buffer.push(item);
      std::cout << "P" << id << " +" << item << " [CAPACITY " << buffer.size()
                << ']' << std::endl;
      sem_post(&item_count); // signal production of item
    }

    pthread_mutex_unlock(&mutex);
    sleep(rand() % 4 + 1); // simulate random production delay
  }

  return nullptr;
}

void *consumer(void *arg) {
  int id = *(int *)arg;
  while (true) {
    sem_wait(&item_count);      // decrease item count
    pthread_mutex_lock(&mutex); // get exclusive access to buffer

    if (!buffer.empty()) {
      int item = buffer.front();
      buffer.pop();
      std::cout << "C" << id << " -" << item << " [CAPACITY " << buffer.size()
                << ']' << std::endl;
    }

    pthread_mutex_unlock(&mutex);
    sleep(rand() % 4 + 1); // simulate random consumption delay
  }
}

int main() {
  pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
  int producer_ids[NUM_PRODUCERS], consumer_ids[NUM_CONSUMERS];

  sem_init(&item_count, 0, 0);
  pthread_mutex_init(&mutex, nullptr);

  for (int i = 0; i < NUM_PRODUCERS; i++) {
    producer_ids[i] = i + 1;
    pthread_create(&producers[i], nullptr, producer, &producer_ids[i]);
  }

  for (int i = 0; i < NUM_CONSUMERS; i++) {
    consumer_ids[i] = i + 1;
    pthread_create(&consumers[i], nullptr, consumer, &consumer_ids[i]);
  }

  for (int i = 0; i < NUM_PRODUCERS; i++)
    pthread_join(producers[i], nullptr);

  for (int i = 0; i < NUM_CONSUMERS; i++)
    pthread_join(consumers[i], nullptr);

  sem_destroy(&item_count);
  pthread_mutex_destroy(&mutex);

  return EXIT_SUCCESS;
}