#ifndef TEA_H
#define TEA_H

#include <stddef.h> // For size_t

// TEA decrypt-function (from David Wheeler og Roger Needham)
void decipher(unsigned int *const v, unsigned int *const w, const unsigned int *const k);

// Check if data is valid
int valid(const unsigned char *data, size_t size);

// Try to find key and decrypt file
int teaDecrypt(const char *input_file, const char *output_file);

#endif // TEA_H


