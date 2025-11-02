#include "file.h"
#include "bin_utils.h"
#include "collections/Vector.h"
#include "Huffman.h"
#include "encoding.h"
#include "serialization.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FILE_BUFFER_LEN 8096

HuffmanNode *huffman_tree_from_file(Vector *arena, FILE *input) {
	int bytes;

	uint8_t table[256];
	memset(table, 0, sizeof(uint8_t) * 256);

	uint8_t in[FILE_BUFFER_LEN];
	memset(in, 0, sizeof(in));

	while ((bytes = fread(in, sizeof(uint8_t), FILE_BUFFER_LEN, input)) > 0) {
		for (int i = 0; i < bytes; i++) {
			table[in[i]]++;
		}
	}

	huffman_nodes_from_table(arena, table);

	fseek(input, 0, SEEK_SET);
	return huffman_build_tree(arena);
}

int huffman_encode_file(HuffmanNode *root, FILE *input, FILE *output) {
	int bytes;

	size_t tree_data_size;
	uint8_t tree_data_buffer[1024];
	uint8_t in[FILE_BUFFER_LEN], out[FILE_BUFFER_LEN];
	memset(out, 0, sizeof(out));

	int written, total_written;
	written = total_written = 0;
	int byte_offset, bit_offset;
	byte_offset = bit_offset = 0;

	Encoder encoder;
	init_encoder(&encoder, root);

	struct TreeData data = tree_data_from_root(root);
	write_tree(&data, tree_data_buffer, sizeof(tree_data_buffer), 0);
	tree_data_size = sizeof(uint16_t) + data.size ;

	byte_offset = tree_data_size;
	bit_offset = tree_data_size * 8;

	while ((bytes = fread(in, sizeof(uint8_t), FILE_BUFFER_LEN, input)) > 0) {
		for (int i = 0; i < bytes; i++) {
			struct Codeword *cw = encoder_get_codeword(&encoder, in[i]);
			written = encoder_write(&encoder, cw, out, FILE_BUFFER_LEN, bit_offset);

			if (written > 0) {
				total_written += written;
				bit_offset += written;
				byte_offset = bit_offset / BYTE_SIZE;
			}

			if (byte_offset >= FILE_BUFFER_LEN - 1) {
				uint8_t last_byte = out[bit_offset / 8];

				fwrite(out, sizeof(uint8_t), FILE_BUFFER_LEN - 1, output);
				memset(out, 0, sizeof(uint8_t));

				bit_offset = bit_offset % 8;
				byte_offset = 0;
				out[0] = last_byte;
			}
		}

		printf("\n");
	}

	if (bit_offset > 0 && bytes < FILE_BUFFER_LEN) {
		fwrite(out, sizeof(uint8_t), byte_offset + 1, output);
		memset(out, 0, sizeof(uint8_t));
	}

	fseek(output, 0, SEEK_SET);

	data.trash = BYTE_SIZE - (bit_offset % BYTE_SIZE);
	tree_data_buffer[0] |= data.trash << 5;
	fwrite(tree_data_buffer, sizeof(uint8_t), tree_data_size, output);

	int written_bytes = tree_data_size + (total_written / 8);
	return written_bytes;
}


int huffman_compress_file(FILE *input, FILE *output) {
	Vector *arena = huffman_arena_new();
	HuffmanNode *root = huffman_tree_from_file(arena, input);

	int result = huffman_encode_file(root, input, output);
	huffman_arena_free(arena);

	return result;
}


int huffman_decompress_file(FILE *input, FILE *output) {
	uint8_t in[FILE_BUFFER_LEN], out[FILE_BUFFER_LEN];
	size_t bytes, byte_idx, out_idx, file_end;

	byte_idx = out_idx = 0;

	memset(out, 0, sizeof(uint8_t) * FILE_BUFFER_LEN);

	fseek(input, 0L, SEEK_END);
	file_end = ftell(input);
	rewind(input);
	
	if (0 > (bytes = fread(in, sizeof(uint8_t), FILE_BUFFER_LEN, input))) {
		puts("Could not read from input file");
		return -1;
	}
	
	Vector *arena = huffman_arena_new();
	struct TreeData data = read_tree(arena, in, FILE_BUFFER_LEN, byte_idx);

	byte_idx += sizeof(uint16_t) + data.size;

	Decoder decoder = new_decoder(data.root);
	Encoder encoder;
	init_encoder(&encoder, data.root);

	uint8_t byte;
	bool end_reached = false;
	putchar('\n');

	while (!end_reached) {
		size_t file_pos = ftell(input);
		end_reached = file_pos == file_end;

		for (; byte_idx < bytes; byte_idx++) {
			decoder_feed(&decoder, in[byte_idx]);

			while (decoder_consume(&decoder, &byte)) {
				if (end_reached && byte_idx == bytes - 1 && decoder.curr_bit == BYTE_SIZE - data.trash) {
					break;
				}
					
				out[out_idx++] = byte;

				if (out_idx >= FILE_BUFFER_LEN) {
					fwrite(out, sizeof(uint8_t), FILE_BUFFER_LEN, output);
					out_idx = 0;
				}
			}
		}
		putchar('\n');

		byte_idx = 0;
		bytes = fread(in, sizeof(uint8_t), FILE_BUFFER_LEN, input);
	}

	if (out_idx > 0) {
		fwrite(out, sizeof(uint8_t), out_idx, output);
		out_idx = 0;
	}
		 
	return 0;
}
