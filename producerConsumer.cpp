#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define THREAD_NUM 8

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;

int buffer[10];
int count = 0;

void printBuffer() {
  std::cout << "Buffer elements: ";
  for (int i = 0; i < count; i++) {
    std::cout << buffer[i] << " ";
  }
  std::cout << std::endl;
}

void *producer(void *args) {
  while (1) {
    // Produce
    int x = rand() % 100;
    sleep(1);

    // Add to the buffer
    sem_wait(&semEmpty);
    pthread_mutex_lock(&mutexBuffer);
    printf("Produced %d\n", x);
    printBuffer();

    buffer[count] = x;
    count++;
    pthread_mutex_unlock(&mutexBuffer);
    sem_post(&semFull);
  }
}

void *consumer(void *args) {
  while (1) {
    int y;

    // Remove from the buffer
    sem_wait(&semFull);
    pthread_mutex_lock(&mutexBuffer);
    y = buffer[count - 1];
    count--;
    pthread_mutex_unlock(&mutexBuffer);
    sem_post(&semEmpty);

    // Consume
    printf("Got %d\n", y);
    printBuffer();
    sleep(1);
  }
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  pthread_t th[THREAD_NUM];
  pthread_mutex_init(&mutexBuffer, NULL);
  sem_init(&semEmpty, 0, 10);
  sem_init(&semFull, 0, 0);
  int i;
  for (i = 0; i < THREAD_NUM; i++) {
    if (i > 0) {
      if (pthread_create(&th[i], NULL, &producer, NULL) != 0) {
        perror("Failed to create thread");
      }
    } else {
      if (pthread_create(&th[i], NULL, &consumer, NULL) != 0) {
        perror("Failed to create thread");
      }
    }
  }
  for (i = 0; i < THREAD_NUM; i++) {
    if (pthread_join(th[i], NULL) != 0) {
      perror("Failed to join thread");
    }
  }
  sem_destroy(&semEmpty);
  sem_destroy(&semFull);
  pthread_mutex_destroy(&mutexBuffer);
  return 0;
}