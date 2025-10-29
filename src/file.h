
#ifndef FILE_H
#define FILE_H

#include <stdio.h>

int huffman_compress_file(FILE *input, FILE *output);
int huffman_decompress_file(FILE *input, FILE *output);



#endif // FILE_H
