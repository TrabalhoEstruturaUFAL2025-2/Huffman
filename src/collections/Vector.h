
#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

#define VECTOR_DEFAULT_CAPACITY 256

typedef struct Vector {
	size_t size;
	size_t capacity;
	size_t element_size;

	void *buffer;
} Vector;

// Creates an empty vector
Vector *vector_new(size_t element_size);

// Destroys a vector so as to avoid memory leaks
void vector_free(Vector *vector);

// Sets *value to the value at the given index. Return -1 on error
int vector_get(Vector *vector, size_t index, void **value);

// Sets the value at the given index to value
int vector_set(Vector *vector, size_t index, void *value);

// Pushes value to the end of the vector
int vector_push(Vector *vector, void *value);

#endif // VECTOR_H
