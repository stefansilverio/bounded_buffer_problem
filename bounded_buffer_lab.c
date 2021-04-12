#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THREAD_NUM 2

sem_t sem_empty;
sem_t sem_full;

pthread_mutex_t mutex_buffer;

int count = 0; // index to write line to, and count of elements currently in buffer
int number_of_lines;  // max number of lines allowed in buffer
char **buffer; // this is the shared buffer of strings 

// args will hold filename of file we need to open
// we know that the buffer should only hold number_of_lines strings
void *producer(void *args)
{
   FILE *fptr;
   size_t max_line_len = 150; // arbitrary

   printf("Name of file we're opening: %s\n\n", (char*)args);

   // open file stream
   if ((fptr = fopen(args, "r")) == NULL)
   {
      printf("Error opening files\n");
      return NULL;
   }

   // dynamically allocate buffer
   printf("Number of bytes allocated: %ld\n\n", (sizeof(char*)*number_of_lines));
   buffer = (malloc(sizeof(char*)*number_of_lines));

   while (1)
   {
      // write next line from opened file into next slot in array of strings
      // otherwise, semaphore should make thread wait 
      sem_wait(&sem_empty);
      pthread_mutex_lock(&mutex_buffer);

      // write string to buffer from file
      getline(&(buffer[count]), &max_line_len, fptr);
      printf("string we just wrote is: %s", buffer[count]);
      count++;
      printf("number of strings in the buffer: %d\n\n", count);
      sleep(1);

      pthread_mutex_unlock(&mutex_buffer);
      sem_post(&sem_full);
   }
   fclose(fptr);
   pthread_exit(0);
}

void *consumer(void *args)
{
   // simulate removing an item from buffer
   // in reality just overwrite it and decrement count
   while (1)
   {
      sem_wait(&sem_full);
      pthread_mutex_lock(&mutex_buffer);

      // print the string at the current index to stdout
      printf("count before we consume string: %d\n", count);
      printf("string we're currently consuming: %s\n", buffer[count-1]);
      count--;

      pthread_mutex_unlock(&mutex_buffer);
      sem_post(&sem_empty); // incrementing number of empty slots
      sleep(1);
   }
   pthread_exit(0);
}

int main(int argc, char *argv[])
{
   pthread_t th[THREAD_NUM]; // thread ids

   // argv[1] is size
   // argv[2] is filename
   // check arg count
   if (argc < 3)
   {
      printf("Supply enough arguments please\n");
      return 1;
   }

   number_of_lines = atoi(argv[1]);
   pthread_mutex_init(&mutex_buffer, NULL);
   sem_init(&sem_empty, 0, number_of_lines); // number_of_lines parameter ensures only ever see that many slots available for writing
   sem_init(&sem_full, 0, 0);

   if (pthread_create(&th[0], NULL, &producer, argv[2]) != 0)
      perror("Failed to create producer thread");

   if (pthread_create(&th[1], NULL, &consumer, NULL) != 0)
      perror("Failed to create consumer thread");
   
   if (pthread_join(th[0], NULL) != 0)
      perror("Failed to join producer thread");
   
   if (pthread_join(th[1], NULL) != 0)
      perror("Failed to join consumer thread");

   sem_destroy(&sem_empty);
   sem_destroy(&sem_full); 
   pthread_mutex_destroy(&mutex_buffer);

   for (int i = 0; i < number_of_lines; i++)
      free(buffer[i]);
   free(buffer);

   return 0;
}