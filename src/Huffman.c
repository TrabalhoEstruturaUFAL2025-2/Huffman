
#include "Huffman.h"
#include "collections/PriorityQueue.h"
#include <stdio.h>

Vector *huffman_arena_new() {
	return vector_new(sizeof(HuffmanNode));
}

void huffman_arena_free(Vector *arena) {
	vector_free(arena);
}

HuffmanNode *huffman_arena_new_leaf(Vector *node_arena, char symbol, int frequency) {
	HuffmanNode leaf;
	leaf.symbol = symbol;
	leaf.frequency = frequency;
	leaf.parent = leaf.left = leaf.right = NULL;

	int err;
	err = vector_push(node_arena, &leaf);
	if (err != 0) {
		return NULL;
	}

	void *node;
	err = vector_get(node_arena, node_arena->size - 1, &node);
	if (err != 0) {
		return NULL;
	}

	return (HuffmanNode *) node;
}

HuffmanNode *huffman_arena_new_internal_node(Vector *node_arena, HuffmanNode *left, HuffmanNode *right) {
	HuffmanNode internal;
	internal.symbol = '*';
	internal.frequency = left->frequency + right->frequency;
	internal.left = left;
	internal.right = right;
	internal.parent = NULL;


	vector_push(node_arena, &internal);

	HuffmanNode *node;
	vector_get(node_arena, node_arena->size - 1, (void **) &node);

	left->parent = right->parent = node;

	return node;
}

int huffman_nodes_from_table(Vector *node_arena, unsigned char table[256]) {
	HuffmanNode leaf;
	int byte_count = 0;

	for (int i = 0; i < 256; i++) {
		if (table[i] > 0) {
			byte_count++;
			if (huffman_arena_new_leaf(node_arena, i, table[i]) == NULL) {
				return -1;
			}
		}
	}

	return byte_count;
}


int huffman_nodes_from_bytes(Vector *node_arena, unsigned char *bytes, size_t size) {
	int err;
	unsigned char table[256];
	memset(table, 0, sizeof(table));

	for (size_t i = 0; i < size; i++) {
		table[bytes[i]]++;
	}

	HuffmanNode leaf;
	int byte_count = 0;

	for (int i = 0; i < 256; i++) {
		if (table[i] > 0) {
			byte_count++;
			if (huffman_arena_new_leaf(node_arena, i, table[i]) == NULL) {
				return -1;
			}
		}
	}

	return byte_count;
}

HuffmanNode *huffman_build_tree(Vector *node_arena) {
	PriorityQueue *queue = priority_queue_new();

	for (size_t i = 0; i < node_arena->size; i++) {
		HuffmanNode *node;
		vector_get(node_arena, i, (void **) &node);
		priority_queue_enqueue(queue, node, node->frequency);
	}

	while (priority_queue_size(queue) > 1) {
		HuffmanNode *left, *right, *internal;

		priority_queue_dequeue(queue, (void **) &left);
		priority_queue_dequeue(queue, (void **) &right);

		internal = huffman_arena_new_internal_node(node_arena, left, right);
		priority_queue_enqueue(queue, internal, internal->frequency);
	}

	HuffmanNode *root;
	priority_queue_dequeue(queue, (void **) &root);
	priority_queue_free(queue);

	return root;
}
