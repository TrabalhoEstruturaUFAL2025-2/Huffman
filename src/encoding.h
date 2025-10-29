
#ifndef ENCODER_H
#define ENCODER_H

#include "Huffman.h"
#include "bin_utils.h"

#include <string.h>

struct Codeword {
	uint8_t bits;
	int length;
};

typedef struct Encoder {
	struct Codeword table[256];
} Encoder;


void init_encoder(Encoder *encoder, HuffmanNode *root);

struct Codeword *encoder_get_codeword(Encoder *encoder, uint8_t byte);

int encoder_write(
	Encoder *encoder, 
	struct Codeword *codeword, 
	uint8_t *buffer, 
	size_t buffer_size,
	int bit_offset
);


typedef struct Decoder {
	uint8_t byte;
	int curr_bit;

	HuffmanNode *root;
	HuffmanNode *current_node;
} Decoder;

Decoder new_decoder(HuffmanNode *root);

int decoder_feed(Decoder *decoder, uint8_t byte);
int decoder_consume(Decoder *decoder, uint8_t *out);

#endif // ENCODER_H
