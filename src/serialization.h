
#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "Huffman.h"
#include <stdint.h>
#include "collections/Vector.h"

void serialize_tree(HuffmanNode *root, uint8_t *buffer, size_t buffer_size);

HuffmanNode *deserialize_tree(Vector *node_arena, uint8_t *buffer, size_t buffer_size);

#endif // SERIALIZATION_H
