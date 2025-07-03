#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h> //Added to include semaphores
#include <stdint.h>
#include <unistd.h>
#include "utils.h"

#define BUFFER_SIZE 4096
#define BYTE_RANGE 256

typedef struct { //Made a structure that will be made in main
    int count[BYTE_RANGE];
    unsigned char buffer[BUFFER_SIZE];
    int bytes_in_buffer;
    pthread_mutex_t mutex;
    sem_t full;   // Edited to use semaphore
    sem_t empty;  // Edited to use semaphore 
    const char* filename; //Added to be able to take in filename in input
    int finished;
} localData; //Gave it a name to be passed on

void* thread_A(void* arg) {
    localData* data = (localData*)arg; //Give access to local variables to be used in threadA
    FILE* fp = fopen(data->filename, "rb"); //Recives filename as argument instead of hardcoded

    if (!fp) {
        perror("Failed to open text file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        sem_wait(&data->empty);                 
        pthread_mutex_lock(&data->mutex);// Edited so all variables uses data before the variable for access

        int read_bytes = fread(data->buffer, 1, BUFFER_SIZE, fp); 
        data->bytes_in_buffer = read_bytes; 

        pthread_mutex_unlock(&data->mutex);
        sem_post(&data->full); //edited to use semaphore

        if (read_bytes < BUFFER_SIZE) {
            break;
        }
    }

    fclose(fp);
    pthread_exit(NULL);
}

void* thread_B(void* arg) { //Recoded thread_B to handle file output hashing
    localData* data = (localData*)arg;
    FILE* enc_out = fopen("task4_pg2265.enc", "wb");
    if (!enc_out) {
        perror("Failed to open encryption output file");
        exit(EXIT_FAILURE);
    }

    
    FILE* hash_in = fopen(data->filename, "rb"); //Opens the input file in binary(rb) mode 
    if (!hash_in) {
        perror("Failed to reopen file for hashing"); //Gives an error if the opening of the file fails
        exit(EXIT_FAILURE);
    }
    int hash_value; //Declares an integer to store the hash value
    Task2_SimpleDjb2Hash(hash_in, &hash_value);
    fclose(hash_in);

    FILE* hash_out = fopen("task4_pg2265.hash", "w"); //Opens/creates hash output file in write mode
    if (!hash_out) {
        perror("Failed to open hash output file"); //Gives error if it fails
        exit(EXIT_FAILURE);
    }
    fprintf(hash_out, "%d\n", hash_value); //Writes the hashed value to file
    fclose(hash_out);

    const unsigned int key[4] = {0x12345678, 0x9ABCDEF0, 0x13579BDF, 0x2468ACE0}; //Unsigned integer keys for tea.c

    while (1) {
        sem_wait(&data->full); //edited to use sem
        pthread_mutex_lock(&data->mutex);

        if (data->bytes_in_buffer == 0) {
            pthread_mutex_unlock(&data->mutex); // Unlock mutex to allow other threads to access shared data
            break;
        }

        int padded_len = data->bytes_in_buffer; // Store the current length of the data in the buffer
        int pad = 8 - (padded_len % 8); // Calculate the padding required
        if (pad == 8) pad = 0;

        if (pad > 0) {
            memset(data->buffer + padded_len, pad, pad); // Adds padding to the buffer
            padded_len += pad;
        }

        for (int i = 0; i < padded_len; i += 8) { // Loop through the padded data in chunks of 8 bytes
            unsigned int v[2]; //Array to hold the encryption
            unsigned int w[2]; // array to hold result of encryption
            memcpy(v, data->buffer + i, 8); // Copy 8 bytes from the buffer into v
            encipher(v, w, key); // Encrypt the 8-byte data using the encryption key
            fwrite(w, sizeof(unsigned int), 2, enc_out); // Write the encrypted data to the output file
        }

        data->bytes_in_buffer = 0; // Empty the buffer byte count after processing
        pthread_mutex_unlock(&data->mutex); // Unlock mutex after processing the data
        sem_post(&data->empty); //Edited to use sem/ Signal that the buffer is now empty and available for the producer thread

        if (padded_len < BUFFER_SIZE) break; // If the padded data is less than the buffer size, exit the loop
    }

    fclose(enc_out);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) { //Edited to take filename as argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pthread_t threadA, threadB;
    localData* data = malloc(sizeof(localData)); //Initialize localData and give it more memory
    if (data == NULL) {
        perror("Failed to allocate memory for localData");
        exit(EXIT_FAILURE);
    }
    data->filename = argv[1]; //Setting filename
    data->finished = 0;

    // Initialize mutex
    if (pthread_mutex_init(&data->mutex, NULL) != 0) {
        perror("Failed to initialize mutex");
        free(data);
        return EXIT_FAILURE;
    }

    // Initializing of semaphores
    if (sem_init(&data->empty, 0, 1) != 0) {// for data at start
        perror("Failed to initialize 'empty' semaphore");
        pthread_mutex_destroy(&data->mutex);
        free(data);
        return EXIT_FAILURE;
    }

    if (sem_init(&data->full, 0, 0) != 0) {//for no data at start                
        perror("Failed to initialize 'full' semaphore");
        sem_destroy(&data->empty);
        pthread_mutex_destroy(&data->mutex);
        free(data);
        return EXIT_FAILURE;
    }

    if (pthread_create(&threadA, NULL, thread_A, data) != 0) { //Edited from thrreadA to threadA
        perror("Could not create thread A");
        exit(1);
    }

    if (pthread_create(&threadB, NULL, thread_B, data) != 0) {
        perror("Could not create thread B");
        exit(1);
    }

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    
    //frees recourses and memory
    pthread_mutex_destroy(&data->mutex); 
    sem_destroy(&data->empty);
    sem_destroy(&data->full);
    free(data);

    return 0;
}
