#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_READERS 12
#define MAX_COUNTER_READS 2000000
#define MAX_COUNTER_WRITES 25000

int counter = 0;
sem_t mutex, wrt;
int reader_count = 0;

void *reader(void *arg) {
    int reader_id = *((int *)arg);
    for (int i = 0; i < MAX_COUNTER_READS; i++) {
        sem_wait(&mutex);
        reader_count++;
        if (reader_count == 1) {
            sem_wait(&wrt);
        }
        sem_post(&mutex);
	



		sem_wait(&mutex);
		reader_count--;
		if (reader_count == 0) {
			sem_post(&wrt);
		}
		sem_post(&mutex);
	}
	
	// Reading the shared counter value
	printf("I'm reader%d, counter = %d\n", reader_id, counter);
    pthread_exit(NULL);
}

void *writer(void *arg) {
    for (int i = 0; i < MAX_COUNTER_WRITES; i++) {
        sem_wait(&wrt);
        // Writing to the shared counter value
        counter += 1;
        sem_post(&wrt);
    }
    printf("Writer Done!\n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int num_readers = atoi(argv[1]);
    int reader_ids[num_readers];
	pthread_t readers[num_readers];
    pthread_t writer_thread;

    if (argc != 2) {
        printf("Usage: %s <number_of_readers>\n", argv[0]);
        return 1;
    }

    if (num_readers < 1 || num_readers > MAX_READERS) {
        printf("Number of readers must be between 1 and %d\n", MAX_READERS);
        return 1;
    }

    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    printf("Executing... please wait...\n");

	pthread_create(&writer_thread, NULL, writer, NULL);

    pthread_join(writer_thread, NULL);

    for (int i = 0; i < num_readers; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    }

    for (int i = 0; i < num_readers; i++) {
        pthread_join(readers[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}

