
#include "encoding.h"
#include "bin_utils.h"
#include <stdio.h>

#define IS_LEAF(node) (node->left == NULL && node->right == NULL)


void _encoder_dfs(Encoder *encoder, HuffmanNode *node, uint8_t byte, int level) {
	if (node->left == NULL && node->right == NULL) {
		uint8_t s = node->symbol;
		encoder->table[s].bits = byte;
		encoder->table[s].length = level;
		printf("symbol: %c, byte: %x\n", s, byte);
	} else {
		_encoder_dfs(encoder, node->left, (byte << 1), level + 1);
		_encoder_dfs(encoder, node->right, (byte << 1) | 1 , level + 1);
	}
}

void init_encoder(Encoder *encoder, HuffmanNode *root) {
	memset(encoder->table, 0, sizeof(struct Codeword) * 256);
	_encoder_dfs(encoder, root, 0, 0);
}

struct Codeword *encoder_get_codeword(Encoder *encoder, uint8_t byte) {
	struct Codeword *cw = &encoder->table[byte];
	if (cw->length == 0) {
		return NULL;
	}

	return cw;
}

int encoder_write(
		Encoder *encoder, 
		struct Codeword *codeword, 
		uint8_t *buffer, 
		size_t buffer_size,
		int offset
	) {
	if (codeword == NULL) {
		return -1;
	}

	if (offset + codeword->length > buffer_size * BYTE_SIZE) {
		return 0;
	}

	write_bits(codeword->bits, codeword->length, buffer, offset);

	return codeword->length;
}

Decoder new_decoder(HuffmanNode *root) {
	Decoder decoder;
	decoder.root = root;
	decoder.current_node = root;
	decoder.byte = 0;
	decoder.curr_bit = -1;
	return decoder;
}

int decoder_feed(Decoder *decoder, uint8_t byte) {
	if (decoder->curr_bit >= 0 && decoder->curr_bit <= BYTE_SIZE - 1) {
		return -1;
	}

	decoder->byte = byte;
	decoder->curr_bit = 0;

	return 0;
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

int decoder_consume(Decoder *decoder, uint8_t *out) {
	while (decoder->curr_bit < BYTE_SIZE) {
		printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(decoder->byte));
		uint8_t bit = NTH_BIT(decoder->byte, decoder->curr_bit);
		printf("\nbit %d number %d\n", bit, decoder->curr_bit);

		if (bit) {
			decoder->current_node = decoder->current_node->right;
		} else {
			decoder->current_node = decoder->current_node->left;
		}

		decoder->curr_bit++;

		if (IS_LEAF(decoder->current_node)) {
			*out = decoder->current_node->symbol;
			decoder->current_node = decoder->root;

			return 1;
		}
	}

	*out = decoder->current_node->symbol;

	return 0;
}
