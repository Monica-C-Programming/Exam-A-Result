/* This file has been created by EWA, and is part of task 4 on the exam for PG3401 2025*/
/* Original code credited to Dan Bernstein*/
#include "utils.h" //Added to use the function


int Task2_SimpleDjb2Hash(FILE * fFileDescriptor, int* piHash) {
   int hash = 5381; 
   int iCharacter = 0; 
   rewind(fFileDescriptor); 
   do {
      iCharacter = fgetc(fFileDescriptor); 
      if (iCharacter == 0 || iCharacter == EOF) break; 
      hash = ((hash << 5) + hash) + (char)iCharacter; /* hash * 33 + c */
   } while (iCharacter != EOF); 
   *piHash = hash; 
   rewind(fFileDescriptor); 
   return 0; 
}
