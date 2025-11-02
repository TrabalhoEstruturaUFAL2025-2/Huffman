
#include "serialization.h"
#include "bin_utils.h"
#include <stdint.h>
#include <stdio.h>

#define ESCAPE_SYMBOL '\\'
#define INTERNAL_SYMBOL '*'

int serialize_recursive(HuffmanNode *node, uint8_t *buffer, size_t len, size_t *i) {
	static char *ERROR = "serialization error: maximum buffer size exceeded\n";

	if (node == NULL) {
		return 0;
	}

	if (*i >= len) goto error;

	if (node->left == NULL && node->right == NULL) {
		if (node->symbol == ESCAPE_SYMBOL || node->symbol == INTERNAL_SYMBOL) {
			buffer[(*i)++] = ESCAPE_SYMBOL;
			if (*i >= len) goto error;
		}

		buffer[(*i)++] = node->symbol;

		return 0;
	} else  {
		buffer[(*i)++] = INTERNAL_SYMBOL;

		if (node->left != NULL) {
			if (serialize_recursive(node->left, buffer, len, i) < 0) goto error;
		} 

		if (node->right != NULL) {
			if (serialize_recursive(node->right, buffer, len, i) < 0) goto error;
		}

		return 0;
	}

	error:
		fprintf(stderr, "%s", ERROR);
		return -1;
}

int serialize_tree(HuffmanNode *root, uint8_t *buffer, size_t buffer_size) {
	size_t i = 0;

	if (serialize_recursive(root, buffer, buffer_size, &i) > 0) {
		return -1;
	}

	return i;
}

HuffmanNode *deserialize_recursive(Vector *node_arena, uint8_t *buffer, size_t len, size_t *i) {
	static char *ERROR = "desserialization error: unexpected end of data\n";

	if (*i >= len) goto error;
	uint8_t symbol = buffer[(*i)++];

	if (symbol != INTERNAL_SYMBOL) {
		if (symbol == ESCAPE_SYMBOL) {
			if (*i >= len) goto error;
			symbol = buffer[(*i)++];
		}

		return huffman_arena_new_leaf(node_arena, symbol, 0);
	} else {
		HuffmanNode *left = deserialize_recursive(node_arena, buffer, len, i);
		if (left == NULL) {
			return NULL;
		}

		HuffmanNode *right = deserialize_recursive(node_arena, buffer, len, i);
		if (right == NULL) {
			return NULL;
		}

		return huffman_arena_new_internal_node(node_arena, left, right);
	}
	
	error:
		fprintf(stderr, "%s", ERROR);
		return NULL;
}

HuffmanNode *deserialize_tree(Vector *node_arena, uint8_t *buffer, size_t len) {
	size_t i = 0;
	return deserialize_recursive(node_arena, buffer, len, &i);
}

struct TreeData tree_data_from_root(HuffmanNode *root) {
	struct TreeData data;

	data.trash = 0;
	data.size = 0;
	data.root = root;

	return data;
}

int write_tree(struct TreeData *data, uint8_t *buffer, size_t len, size_t offset) {
	int bytes = serialize_tree(data->root, buffer + offset + 2, len - 2);
	if (bytes < 0) {
		return -1;
	}

	data->size = bytes;
	
	buffer[offset] = (uint8_t) ((data->size >> 8) | (data->trash << 5));
	buffer[offset + 1] = (uint8_t) (data->size & 0xFF);

	return sizeof(uint16_t) + bytes;
}

struct TreeData read_tree(Vector *node_arena, uint8_t *buffer, size_t len, size_t offset) {
	struct TreeData data;
	data.size = ((buffer[offset] & 0b00011111) << 8) | buffer[offset + 1];
	data.trash = buffer[offset] >> (BYTE_SIZE - 3);
	data.root = deserialize_tree(node_arena, buffer + 2, data.size);
	return data;
}
