
#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "Huffman.h"
#include <stdint.h>
#include "collections/Vector.h"

struct TreeData {
	uint8_t trash:3;
	uint16_t size:13;
	HuffmanNode *root;
};

int serialize_tree(HuffmanNode *root, uint8_t *buffer, size_t buffer_size);

HuffmanNode *deserialize_tree(Vector *node_arena, uint8_t *buffer, size_t len);

struct TreeData get_tree_data(HuffmanNode *root);

int write_tree(struct TreeData *tree_data, uint8_t *buffer, size_t len, size_t offset);
struct TreeData read_tree(Vector *node_arena, uint8_t *buffer, size_t len, size_t offset);

#endif // SERIALIZATION_H
