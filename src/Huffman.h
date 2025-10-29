
#ifndef HUFFMANNODE_H
#define HUFFMANNODE_H

#include "collections/PriorityQueue.h"
#include "collections/Vector.h"

/**
 * @struct HuffmanNode
 * @brief Estrutura que representa um nó na árvore de Huffman.
 * 
 * Todos os nós são armazenados em um vetor (arena de memória) para melhorar
 * a localidade de memória e reduzir fragmentação.
 */
typedef struct HuffmanNode {
    /** 
     * @brief Símbolo armazenado no nó.
     * @details Válido apenas para nós folha. Nós internos não utilizam este campo.
     */
    char symbol;
    
    /** 
     * @brief Frequência do símbolo ou soma das frequências dos filhos.
     * @details Para nós folha: frequência de ocorrência do símbolo no texto.
     *          Para nós internos: soma das frequências dos nós filhos.
     */
    int frequency;
    
    /** 
     * @brief Ponteiro para o nó pai na árvore.
     * @details NULL para o nó raiz da árvore.
     */
    struct HuffmanNode *parent;
    
    /** 
     * @brief Ponteiro para o filho esquerdo.
     * @details NULL para nós folha. Nós internos sempre possuem dois filhos.
     */
    struct HuffmanNode *left;
    
    /** 
     * @brief Ponteiro para o filho direito.
     * @details NULL para nós folha. Nós internos sempre possuem dois filhos.
     */
    struct HuffmanNode *right;
} HuffmanNode;

/**
 * @brief Cria uma nova arena de memória para armazenar nós de Huffman.
 * 
 * @details Aloca um vetor (Vector) que servirá como arena de memória para
 * todos os nós da árvore de Huffman. Esta abordagem melhora a localidade
 * de memória, reduz fragmentação e facilita o gerenciamento de memória.
 * 
 * @return Ponteiro para o Vector criado em caso de sucesso, ou NULL se 
 *         houver falha na alocação de memória.
 * 
 * @note A arena retornada deve ser liberada utilizando huffman_node_arena_free()
 *       após o uso para evitar vazamento de memória.
 * 
 * @see huffman_node_arena_free()
 */
Vector *huffman_arena_new();

/**
 * @brief Libera a memória alocada para a arena de nós de Huffman.
 * 
 * @details Desaloca toda a memória utilizada pela arena, incluindo todos
 * os nós de Huffman armazenados nela. Após chamar esta função, todos os
 * ponteiros para nós dentro desta arena tornam-se inválidos.
 * 
 * @param node_arena Ponteiro para a arena de memória a ser liberada.
 *                   Se NULL, a função não realiza nenhuma operação.
 * 
 * @warning Após chamar esta função, não utilize nenhum ponteiro para nós
 *          que estavam armazenados nesta arena.
 * 
 * @see huffman_node_arena_new()
 */
void huffman_arena_free(Vector *node_arena);

/**
 * @brief Cria um novo nó folha na arena de memória.
 * 
 * @details Aloca e inicializa um nó folha na arena especificada. Nós folha
 * são nós terminais da árvore de Huffman que contêm um símbolo específico
 * e sua frequência de ocorrência. Eles não possuem filhos (left e right são NULL).
 * 
 * @param node_arena Arena de memória onde o nó será alocado. Não pode ser NULL.
 * @param symbol Caractere/símbolo que este nó folha representa.
 * @param frequency Frequência de ocorrência do símbolo no texto original.
 *                  Deve ser um valor positivo.
 * 
 * @return Ponteiro para o nó folha criado em caso de sucesso, ou NULL se
 *         houver falha na alocação ou se node_arena for NULL.
 * 
 * @note O nó retornado terá:
 *       - symbol: o símbolo fornecido
 *       - frequency: a frequência fornecida
 *       - parent, left, right: todos inicializados como NULL
 * 
 * @see huffman_node_arena_new_internal()
 */
HuffmanNode *huffman_arena_new_leaf(Vector *node_arena, char symbol, int frequency);

/**
 * @brief Cria um novo nó interno na arena de memória.
 * 
 * @details Aloca e inicializa um nó interno na arena especificada. Nós internos
 * são nós não-terminais da árvore de Huffman que conectam outros nós (folhas ou
 * internos). Eles não possuem símbolo próprio, mas sua frequência é a soma das
 * frequências de seus filhos.
 * 
 * @param node_arena Arena de memória onde o nó será alocado. Não pode ser NULL.
 * @param left Ponteiro para o nó que será o filho esquerdo. Não pode ser NULL.
 * @param right Ponteiro para o nó que será o filho direito. Não pode ser NULL.
 * 
 * @return Ponteiro para o nó interno criado em caso de sucesso, ou NULL se
 *         houver falha na alocação ou se algum parâmetro for NULL.
 * 
 * @note O nó retornado terá:
 *       - frequency: soma de left->frequency + right->frequency
 *       - left e right: apontando para os nós fornecidos
 *       - parent: inicializado como NULL
 *       - symbol: não definido (nós internos não usam este campo)
 * 
 * @note Esta função também atualiza o campo parent dos nós filhos para
 *       apontar para o novo nó interno criado.
 * 
 * @see huffman_arena_new_leaf()
 */
HuffmanNode *huffman_arena_new_internal_node(Vector *node_arena, HuffmanNode *left, HuffmanNode *right);

/**
 * @brief Cria nós folha a partir de um array de bytes.
 * 
 * @details Analisa o array de bytes fornecido, calcula a frequência de cada
 * byte único e cria um nó folha para cada byte distinto encontrado. Esta é
 * tipicamente a primeira etapa na construção de uma árvore de Huffman.
 * 
 * @param node_arena Arena de memória onde os nós serão alocados. Não pode ser NULL.
 * @param bytes Array de bytes a ser analisado. Não pode ser NULL.
 * @param size Tamanho do array de bytes. Deve ser maior que 0.
 * 
 * @return Número de nós folha criados (valor não-negativo) em caso de sucesso,
 *         representando a quantidade de bytes distintos encontrados no array.
 *         Retorna um valor negativo em caso de erro.
 * 
 * @note Esta função criará no máximo 256 nós folha (um para cada possível
 *       valor de byte de 0 a 255).
 * 
 * @example
 * @code
 * Vector *arena = huffman_node_arena_new();
 * unsigned char texto[] = "AABBCCCC";
 * int num_nos = huffman_nodes_from_bytes(arena, texto, 8);
 * // num_nos será 3 (nós para 'A', 'B' e 'C')
 * // Nó 'A' terá frequência 2
 * // Nó 'B' terá frequência 2
 * // Nó 'C' terá frequência 4
 * @endcode
 * 
 * @see huffman_arena_new_leaf()
 */
int huffman_nodes_from_bytes(Vector *node_arena, unsigned char *bytes, size_t size);


/**
 * @brief Cria nós folha a partir de um array de bytes.
 * 
 * @details Analisa uma tabela em formato de array contendo a frequência de cada
 * byte possível e cria um nó folha para cada byte distinto. Semelhante a huffman_nodes_from_bytes
 * porém delegando a etapa de contar a frequência dos bytes no arquivo
 * 
 * @param node_arena Arena de memória onde os nós serão alocados. Não pode ser NULL.
 * @param bytes Tabela de frequência a ser analisada. Não pode ser NULL.
 * @param size Tamanho do array de bytes. Deve ser maior que 0.
 * 
 * @return Número de nós folha criados (valor não-negativo) em caso de sucesso,
 *         representando a quantidade de bytes com frequência > 0 no array.
 *         Retorna um valor negativo em caso de erro.
 * 
 * @note Esta função criará no máximo 256 nós folha (um para cada possível
 *       valor de byte de 0 a 255).
 * 
 * @see huffman_arena_new_leaf()
 * @see huffman_nodes_from_bytes()
 */
 int huffman_nodes_from_table(Vector *node_arena, unsigned char table[256]);

/**
 * @brief Constrói a árvore de Huffman a partir dos nós folha na arena.
 * 
 * @details Esta função implementa o algoritmo de Huffman para construir uma
 * árvore binária ótima de codificação. O algoritmo funciona da seguinte forma:
 * 1. Insere todos os nós folha existentes na arena em uma fila de prioridade,
 *    ordenados por frequência crescente.
 * 2. Repetidamente remove os dois nós de menor frequência da fila.
 * 3. Cria um novo nó interno com esses dois nós como filhos.
 * 4. Insere o novo nó interno de volta na fila.
 * 5. Repete os passos 2-4 até restar apenas um nó na fila, que será a raiz.
 * 
 * A árvore resultante garante que símbolos mais frequentes tenham códigos
 * mais curtos, otimizando a compressão de dados.
 * 
 * @param node_arena Arena de memória contendo os nós folha previamente criados
 *                   por huffman_nodes_from_bytes() ou huffman_nodes_from_table().
 *                   Não pode ser NULL e deve conter ao menos um nó folha.
 * 
 * @return Ponteiro para o nó raiz da árvore de Huffman construída em caso de
 *         sucesso, ou NULL em caso de erro (arena NULL, arena vazia, ou falha
 *         de alocação durante a construção).
 * 
 * @note A arena deve conter apenas nós folha antes de chamar esta função.
 *       Nós internos serão criados e adicionados à arena durante a execução.
 * 
 * @note Para um arquivo com N símbolos distintos, a árvore resultante terá:
 *       - N nós folha (já existentes na arena)
 *       - N-1 nós internos (criados por esta função)
 *       - Total de 2N-1 nós
 * 
 * @note Caso especial: se houver apenas um símbolo distinto no arquivo,
 *       a função deve tratar este caso criando uma árvore mínima válida.
 * 
 * @warning Não modifique a arena entre a criação dos nós folha e a chamada
 *          desta função, pois isso pode resultar em comportamento indefinido.
 * 
 * @example
 * @code
 * Vector *arena = huffman_arena_new();
 * unsigned char data[] = "AABBCCCC";
 * huffman_nodes_from_bytes(arena, data, 8);
 * HuffmanNode *root = huffman_build_tree(arena);
 * if (root != NULL) {
 *     // Árvore construída com sucesso
 *     // root->frequency será 8 (soma de todas as frequências)
 * }
 * huffman_arena_free(arena);
 * @endcode
 * 
 * @see huffman_nodes_from_bytes()
 * @see huffman_nodes_from_table()
 * @see huffman_arena_new_internal_node()
 */HuffmanNode *huffman_build_tree(Vector *node_arena);
#endif // HUFFMANNODE_H
