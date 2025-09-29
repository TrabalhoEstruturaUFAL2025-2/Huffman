#include "Vector.h"

Vector *vector_new(size_t element_size) {
    Vector *vector = (Vector*) malloc(sizeof(Vector));

    vector->size = 0;
    vector->capacity = 0;
    vector->element_size = element_size;
    vector->buffer = NULL;

    return vector;
}

void vector_free(Vector *vector) {
    free(vector->buffer);
    free(vector);
}

// Faz com que value aponte para um endereço calculado com base em um index
int vector_get(Vector *vector, size_t index, void **value) {
    // Retorna -1 em caso de erro
    if (!vector || !value || index>= vector->size) {
        return -1;
    }
    // Converte o buffer para 'char *' para que se mova de 1 em 1 byte na memória
    // e calcula o endereço exato do item
    *value = (char *)vector->buffer + (index * vector->element_size);
    return 0;
}

// Copia o conteudo de value para o endereço de destino dentro do buffer
int vector_set(Vector *vector, size_t index, void *value) {
    // Retorna -1 em caso de erro
    if (!vector || !value || index>= vector->size) {
        return -1;
    }
        
    // Converte o buffer para 'char *' para que se mova de 1 em 1 byte na memória
    void *dest = (char *)vector->buffer + (index * vector->element_size);
    
    memcpy(dest, value, vector->element_size);
    return 0;
}

// Insere no final do vetor
int vector_push(Vector *vector, void *value) {
    if (!vector || !value) {
        return -1;
    }

    // Dobra o tamanho caso precise
    if (vector->size >= vector->capacity) {
        size_t new_capacity = (vector->capacity == 0) ? 
			VECTOR_DEFAULT_CAPACITY : 
			vector->capacity * 2;
        void *new_buffer = realloc(vector->buffer, new_capacity * vector->element_size);
        if (!new_buffer) {
            return -1;
        }
        vector->buffer = new_buffer;
        vector->capacity = new_capacity;
    }

    // Insere no final do vetor
    void *dest = (char *)vector->buffer + (vector->size * vector->element_size);
    memcpy(dest, value, vector->element_size);
    vector->size++;
    return 0;
}

int vector_clear(Vector *vector) {
	if (!vector) {
		return -1;
	}
	vector->size = 0;
	return 0;
}
