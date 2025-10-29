#include "file.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	if (argc != 4) {
		printf(
			"Usage: %s <compress|decompress> <input_file> <output_file>\n", 
			argv[0]
		);

		return 1;
	}

	char *command = argv[1];
	char *input_path = argv[2];
	char *output_path = argv[3];

	FILE *in = fopen(input_path, "rb");
	if (in == NULL) {
		printf("Error opening file %s\n", input_path);
		return 1;
	}

	FILE *out = fopen(output_path, "wb");
	if (out == NULL) {
		printf("Error opening file %s\n", output_path);
		return 1;
	}

	if (strcmp(argv[1], "compress") == 0) {
		printf("Compressing %s to %s\n", input_path, output_path);
		int bytes = huffman_compress_file(in, out);

		fclose(in);
		fclose(out);

		if (bytes > 0) {
			printf("Compressed file into %d bytes\n", bytes);
			return 0;
		}		

		return -1;
	} else if (strcmp(argv[1], "decompress") == 0) {
		huffman_decompress_file(in, out);

		fclose(in);
		fclose(out);
	} else {
		printf("Unknown command %s\n", command);
		return 1;
	}

	return 0;
}
