// utils.h

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

// Hash function from dbj2.c
int Task2_SimpleDjb2Hash(FILE * fFileDescriptor, int* piHash);

//Encrypting from tea.c
void encipher(unsigned int *const v, unsigned int *const w, const unsigned int *const k);

#endif // UTILS_H

