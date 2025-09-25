
#ifndef HUFFMANNODE_H
#define HUFFMANNODE_H

typedef struct HuffmanNode {
	char symbol;
	int frequency;

	struct HuffmanNode *parent;
	struct HuffmanNode *left;
	struct HuffmanNode *right;
} HuffmanNode;

// Sets the fields of the node. Left and Right are set to NULL
void huffman_node_leaf_init(HuffmanNode *node, char symbol, int frequency);

void huffman_node_combine(HuffmanNode *parent, HuffmanNode *left, HuffmanNode *right);

#endif // HUFFMANNODE_H
