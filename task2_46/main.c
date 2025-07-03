#include <stdio.h>
#include <string.h>
#include "task2_count.h"
#include "task2_hash.h"
#include "task2_sum.h"
#include "task2_metadata.h"

//My main function for opening and reading the textfile
int main() {
    FILE* f = fopen("pgexam25_test.txt", "rb");
    if (!f) {
        perror("The textfile could not be opened, sorry!");
        return 1;
    }

    struct TASK2_FILE_METADATA metadata = { 0 }; //Initializes the data to 0 before starting the program
    strncpy(metadata.szFileName, "pgexam25_test.txt", sizeof(metadata.szFileName) - 1);

// Method for linking the count file
    memset(metadata.aAlphaCount, 0, sizeof(metadata.aAlphaCount));
    Task2_CountEachCharacter(f, metadata.aAlphaCount);

// Method for linking the hash file
    unsigned int hash = 0;
    Task2_SimpleDjb2Hash(f, &hash);
    memcpy(metadata.byHash, &hash, sizeof(metadata.byHash));

// Method for linking the sum file
    Task2_SizeAndSumOfCharacters(f, &metadata.iFileSize, &metadata.iSumOfChars);


    fclose(f); //Closing the filereader to prevent leaking

    // Function to write the structure to output file as binary
    FILE* fout = fopen("pgexam25_output.bin", "wb");
    if (!fout) {
        perror("Failed to create the output file");
        return 1;
    }

    fwrite(&metadata, sizeof(struct TASK2_FILE_METADATA), 1, fout);
    fclose(fout); //Close the reader to prevent leaking

    printf("The file was written to pgexam25_output.bin\n");
    return 0;
}

