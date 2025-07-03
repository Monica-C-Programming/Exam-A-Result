
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tea.h"

//Void decipher is not written by me, see comment for source:
/** Original code credited to David Wheeler and Roger Needham */
/* Documentation: https://www.schneier.com/wp-content/uploads/2015/03/TEA-2.c */ 
// Used the decrypt from David Wheeler and Roger Needham
void decipher(unsigned int *const v,unsigned int *const w,
    const unsigned int *const k) //Edited from long to int as the hint in the task said to do
    {
       register unsigned int       y=v[0],z=v[1],sum=0xC6EF3720,
                    delta=0x9E3779B9,a=k[0],b=k[1],c=k[2],
                    d=k[3],n=32;
    
       while(n-->0)
          {
          z -= (y<<4)+c ^ y+sum ^ (y>>5)+d;
          y -= (z<<4)+a ^ z+sum ^ (z>>5)+b;
          sum -= delta;
          }
       
       w[0]=y; w[1]=z;
    }
    //My code
    //Function to check if the data is valid and can be used for our task
    int valid(const unsigned char *data, size_t size) {
        for (size_t i = 0; i < size; i++) {
            if (data[i] < 0x20 || data[i] > 0x7E) {
                return 0; //Exits if not valid
            }
        }
        return 1; // Valid ASCII
    }
    


    int teaDecrypt(const char *input_file, const char *output_file) { //Function to try to find the right key
        FILE *f = fopen(input_file, "rb"); //Check that we actually have a file to decrypt and prints error if not
        if (!f) {
            perror("Could not open encrypted file");
            return 1;
        }
    
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        rewind(f);
    
        unsigned char *ciphertext = malloc(size); //Allocate memory for the file we will create
        if (!ciphertext) {
            perror("Memory allocation failed");
            fclose(f);
            return 1;
        }
        fread(ciphertext, 1, size, f);
        fclose(f);
    
        unsigned int block_in[2], block_out[2];
        unsigned char *output = malloc(size);
    
        for (int i = 0; i < 256; ++i) { //For loop to try all possible keys that our encryption can use
            unsigned char byte = (unsigned char)i;
            unsigned int key[4];
            for (int j = 0; j < 4; ++j) {
                key[j] = byte | (byte << 8) | (byte << 16) | (byte << 24);
            }
    
            memcpy(block_in, ciphertext, 8);
            decipher(block_in, block_out, key);
            memcpy(output, block_out, 8);
    
            if (valid(output, 8)) { //Check to see if we found a valid key, and prints if we find a match
                printf("Matching key found: 0x%02X\n", byte);
                for (int pos = 0; pos < size; pos += 8) {
                    memcpy(block_in, ciphertext + pos, 8);
                    decipher(block_in, block_out, key);
                    memcpy(output + pos, block_out, 8);
                }
    
                FILE *out = fopen(output_file, "wb"); //Prints the text to a textfile if possible, if not we get an error
                if (!out) {
                    perror("Could not open file"); //Error if it fails
                    free(ciphertext);
                    free(output);
                    return 1;
                }
                fwrite(output, 1, size, out);
                fclose(out);
                printf("Decrypt success, Saved to '%s'\n", output_file); //Prints out where the saved data is saved to
    
                free(ciphertext);
                free(output);
                return 0;
            }
        }
    
        printf("No key found.\n"); //If no key is found

        //Frees recourses
        free(ciphertext);
        free(output);
        return 1;
    }
