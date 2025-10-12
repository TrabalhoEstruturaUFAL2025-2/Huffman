
#include "serialization.h"
#include <stdio.h>

#define INTERNAL_MARKER '*'
#define LEAF_MARKER 1

int serialize_recursive(HuffmanNode *node, uint8_t *buffer, size_t len, size_t *i) {
	if (node == NULL) {
		return 0;
	}

	printf("index: %d\n", (int) *i);

	if (*i >= len) {
		fprintf(stderr, "serialization error: maximum buffer size exceeded\n");
		return -1;
	}

	buffer[*i] = INTERNAL_MARKER;
	(*i)++;

	if (node->left != NULL) {
		serialize_recursive(node->left, buffer, len, i);
	} 

	if (node->right != NULL) {
		serialize_recursive(node->right, buffer, len, i);
	}

	if (node->left == NULL && node->right == NULL) {
		buffer[(*i - 1)] = LEAF_MARKER;
		
		if (*i >= len) {
			fprintf(stderr, "serialization error: maximum buffer size exceeded\n");
			return -1;
		}

		buffer[*i] = node->symbol;
		(*i)++;
	}

	return 0;
}

void serialize_tree(HuffmanNode *root, uint8_t *buffer, size_t buffer_size) {
	puts("=====");
	size_t i = 0;
	serialize_recursive(root, buffer, buffer_size, &i);
}

HuffmanNode *deserialize_recursive(Vector *node_arena, uint8_t *buffer, size_t len, size_t *i) {
	if (*i >= len) {
		fprintf(stderr, "desserialization error: unexpected end of data\n");
		return NULL;
	}

	uint8_t marker = buffer[*i];
	(*i)++;

	if (marker == LEAF_MARKER) {
		if (*i >= len) {
			fprintf(stderr, "desserialization error: unexpected end of data\n");
			return NULL;
		}

		char symbol = buffer[*i];
		(*i)++;
		return huffman_arena_new_leaf(node_arena, symbol, 0);
	} else if (marker == INTERNAL_MARKER) {
		HuffmanNode *left = deserialize_recursive(node_arena, buffer, len, i);
		if (left == NULL) {
			return NULL;
		}

		HuffmanNode *right = deserialize_recursive(node_arena, buffer, len, i);
		if (right == NULL) {
			return NULL;
		}

		return huffman_arena_new_internal_node(node_arena, left, right);
	} else {
		fprintf(stderr, "desserialization error: unexpected marker 0x%02X\n", marker);
		return NULL;
	}
}

HuffmanNode *deserialize_tree(Vector *node_arena, uint8_t *buffer, size_t buffer_size) {
	size_t i = 0;
	return deserialize_recursive(node_arena, buffer, buffer_size, &i);
}
