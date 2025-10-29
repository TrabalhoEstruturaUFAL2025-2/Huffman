#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "Huffman.h"
#include <stdint.h>
#include "collections/Vector.h"

/**
 * @struct TreeData
 * @brief Estrutura que encapsula metadados e a raiz da árvore de Huffman.
 * 
 * @details Contém informações essenciais sobre a árvore de Huffman e o arquivo
 * comprimido, incluindo o número de bits não utilizados no último byte (trash)
 * e o tamanho da representação serializada da árvore. Os campos trash e size
 * são compactados em bit fields para economizar espaço, totalizando 2 bytes.
 */
struct TreeData {
    /**
     * @brief Número de bits de preenchimento (trash) no último byte do arquivo.
     * @details Indica quantos bits do último byte do arquivo comprimido não
     * contêm dados válidos. Varia de 0 a 7. Este valor é necessário para saber
     * exatamente onde os dados codificados terminam, evitando decodificar bits
     * de padding.
     * 
     * Exemplo: Se o arquivo comprimido tem 43 bits de dados úteis, serão usados
     * 6 bytes (48 bits), e trash será 5 (48 - 43 = 5 bits não utilizados).
     */
    uint8_t trash:3;
    
    /**
     * @brief Tamanho da árvore serializada em bytes.
     * @details Indica quantos bytes são necessários para armazenar a representação
     * serializada da árvore de Huffman. Este valor permite localizar onde os dados
     * comprimidos começam no arquivo (após o cabeçalho e a árvore). Limitado a
     * 8191 bytes (2^13 - 1) devido ao bit field de 13 bits.
     */
    uint16_t size:13;
    
    /**
     * @brief Ponteiro para o nó raiz da árvore de Huffman.
     * @details Aponta para a raiz da árvore de Huffman reconstruída ou a ser
     * serializada. NULL se a árvore ainda não foi construída ou em caso de erro.
     */
    HuffmanNode *root;
};

/**
 * @brief Obtém os metadados da árvore de Huffman.
 * 
 * @details Calcula o tamanho que a árvore ocupará quando serializada percorrendo-a
 * a partir da raiz. Retorna uma estrutura TreeData contendo o tamanho calculado
 * e a raiz fornecida. O campo trash não é calculado por esta função (deve ser
 * definido separadamente baseado nos dados comprimidos).
 * 
 * O tamanho é calculado baseado no formato de serialização específico implementado
 * em serialize_tree().
 * 
 * @param root Ponteiro para o nó raiz da árvore de Huffman. Não pode ser NULL.
 * 
 * @return Estrutura TreeData contendo:
 *         - size: tamanho em bytes que a árvore ocupará quando serializada
 *         - root: o ponteiro root fornecido
 *         - trash: indefinido (deve ser definido pelo chamador se necessário)
 * 
 * @note Esta função apenas calcula o tamanho, não serializa a árvore.
 * 
 * @note Se root for NULL, o comportamento é indefinido.
 * 
 * @example
 * @code
 * HuffmanNode *root = huffman_build_tree(arena);
 * struct TreeData tree_data = get_tree_data(root);
 * printf("Árvore ocupará %u bytes quando serializada\n", tree_data.size);
 * @endcode
 * 
 * @see serialize_tree()
 * @see write_tree()
 */
struct TreeData get_tree_data(HuffmanNode *root);

/**
 * @brief Serializa a árvore de Huffman em um buffer.
 * 
 * @details Converte a árvore de Huffman em uma representação linear (flat)
 * armazenada no buffer. A árvore é percorrida e codificada em um formato
 * específico que permite reconstrução posterior via deserialize_tree().
 * 
 * O formato de serialização típico usa:
 * - '*' ou '1' para indicar nó interno
 * - '0' seguido do byte do símbolo para indicar nó folha
 * 
 * Exemplo: A árvore com símbolos 'A' e 'B' poderia ser serializada como:
 * "*0A0B" (nó interno com dois filhos folha)
 * 
 * @param root Ponteiro para o nó raiz da árvore a ser serializada. Não pode ser NULL.
 * @param buffer Buffer de destino onde a árvore serializada será escrita.
 *               Não pode ser NULL.
 * @param buffer_size Tamanho do buffer em bytes. Deve ser suficiente para
 *                    acomodar toda a árvore serializada.
 * 
 * @return Número de bytes escritos no buffer (> 0) em caso de sucesso.
 *         Valor negativo em caso de erro (parâmetros NULL, buffer insuficiente,
 *         ou árvore inválida).
 * 
 * @note Use get_tree_data() para determinar o tamanho necessário do buffer
 *       antes de chamar esta função.
 * 
 * @note A árvore é serializada usando pré-ordem (pre-order traversal).
 * 
 * @warning O buffer deve ter espaço suficiente. Se buffer_size for menor
 *          que o necessário, a função retornará erro.
 * 
 * @example
 * @code
 * HuffmanNode *root = huffman_build_tree(arena);
 * struct TreeData tree_data = get_tree_data(root);
 * 
 * uint8_t *buffer = malloc(tree_data.size);
 * int bytes_written = serialize_tree(root, buffer, tree_data.size);
 * 
 * if (bytes_written > 0) {
 *     printf("Árvore serializada: %d bytes\n", bytes_written);
 * }
 * @endcode
 * 
 * @see deserialize_tree()
 * @see get_tree_data()
 * @see write_tree()
 */
int serialize_tree(HuffmanNode *root, uint8_t *buffer, size_t buffer_size);

/**
 * @brief Desserializa uma árvore de Huffman a partir de um buffer.
 * 
 * @details Reconstrói a árvore de Huffman a partir de sua representação
 * serializada no buffer. Cria novos nós na arena fornecida conforme lê
 * os dados serializados, reconstruindo a estrutura original da árvore.
 * 
 * O formato esperado deve corresponder ao gerado por serialize_tree().
 * A função interpreta os marcadores de nós internos e folhas, recriando
 * a árvore de forma recursiva ou iterativa.
 * 
 * @param node_arena Arena de memória onde os novos nós serão alocados.
 *                   Não pode ser NULL.
 * @param buffer Buffer contendo a árvore serializada. Não pode ser NULL.
 * @param len Tamanho dos dados serializados no buffer em bytes.
 *            Deve corresponder ao tamanho da árvore serializada.
 * 
 * @return Ponteiro para o nó raiz da árvore reconstruída em caso de sucesso.
 *         NULL em caso de erro (parâmetros NULL, dados corrompidos, formato
 *         inválido, ou falha de alocação na arena).
 * 
 * @note Os nós criados são alocados na arena fornecida e devem ser liberados
 *       via huffman_arena_free() quando não forem mais necessários.
 * 
 * @note A arena deve estar vazia ou preparada para receber novos nós antes
 *       de chamar esta função.
 * 
 * @warning Se o buffer contiver dados corrompidos ou em formato inválido,
 *          o comportamento é indefinido. A função pode retornar NULL ou
 *          criar uma árvore parcial/incorreta.
 * 
 * @example
 * @code
 * Vector *arena = huffman_arena_new();
 * 
 * // buffer contém árvore serializada lida de arquivo
 * HuffmanNode *root = deserialize_tree(arena, buffer, tree_size);
 * 
 * if (root != NULL) {
 *     // Árvore reconstruída com sucesso
 *     Decoder decoder = new_decoder(root);
 *     // ... usar decoder ...
 * }
 * 
 * huffman_arena_free(arena);
 * @endcode
 * 
 * @see serialize_tree()
 * @see read_tree()
 * @see huffman_arena_new()
 */
HuffmanNode *deserialize_tree(Vector *node_arena, uint8_t *buffer, size_t len);

/**
 * @brief Escreve a árvore e seus metadados em um buffer.
 * 
 * @details Serializa a árvore de Huffman e seus metadados (trash e size) em
 * um buffer, começando em um offset específico. Esta função combina:
 * 1. Escrita dos 2 bytes de cabeçalho contendo trash (3 bits) e size (13 bits)
 * 2. Serialização da árvore usando serialize_tree()
 * 
 * O formato do cabeçalho de 2 bytes:
 * - Byte 0: [trash(3 bits)][size(5 bits mais significativos)]
 * - Byte 1: [size(8 bits menos significativos)]
 * 
 * Este é o formato típico usado em arquivos comprimidos Huffman, onde o
 * cabeçalho precede a árvore serializada.
 * 
 * @param tree_data Ponteiro para estrutura TreeData contendo os metadados e
 *                  a raiz da árvore. Não pode ser NULL e tree_data->root não
 *                  pode ser NULL.
 * @param buffer Buffer de destino onde os dados serão escritos. Não pode ser NULL.
 * @param len Tamanho total do buffer em bytes. Deve ser suficiente para
 *            acomodar offset + 2 bytes de cabeçalho + tamanho da árvore.
 * @param offset Posição inicial no buffer (em bytes) onde a escrita começará.
 *               Geralmente 0 para escrever no início do buffer.
 * 
 * @return Número total de bytes escritos (cabeçalho + árvore) em caso de sucesso.
 *         Valor negativo em caso de erro (parâmetros NULL, buffer insuficiente,
 *         offset inválido, ou falha na serialização).
 * 
 * @note O valor retornado inclui os 2 bytes do cabeçalho mais o tamanho da
 *       árvore serializada.
 * 
 * @note tree_data->size deve estar corretamente preenchido antes de chamar
 *       esta função (use get_tree_data() se necessário).
 * 
 * @example
 * @code
 * HuffmanNode *root = huffman_build_tree(arena);
 * struct TreeData tree_data = get_tree_data(root);
 * tree_data.trash = 5; // Definir trash bits conforme necessário
 * 
 * // Buffer para cabeçalho (2 bytes) + árvore
 * size_t total_size = 2 + tree_data.size;
 * uint8_t *buffer = malloc(total_size);
 * 
 * int bytes_written = write_tree(&tree_data, buffer, total_size, 0);
 * if (bytes_written > 0) {
 *     printf("Cabeçalho + árvore: %d bytes\n", bytes_written);
 *     // buffer[0] e buffer[1] contêm trash e size
 *     // buffer[2...] contém a árvore serializada
 * }
 * @endcode
 * 
 * @see read_tree()
 * @see serialize_tree()
 * @see get_tree_data()
 */
int write_tree(struct TreeData *tree_data, uint8_t *buffer, size_t len, size_t offset);

/**
 * @brief Lê a árvore e seus metadados de um buffer.
 * 
 * @details Desserializa a árvore de Huffman e extrai seus metadados (trash e size)
 * a partir de um buffer, começando em um offset específico. Esta função:
 * 1. Lê os 2 bytes de cabeçalho, extraindo trash (3 bits) e size (13 bits)
 * 2. Desserializa a árvore usando deserialize_tree()
 * 3. Retorna uma estrutura TreeData completa
 * 
 * O formato esperado do cabeçalho de 2 bytes:
 * - Byte 0: [trash(3 bits)][size(5 bits mais significativos)]
 * - Byte 1: [size(8 bits menos significativos)]
 * 
 * Esta função é usada para ler arquivos comprimidos Huffman, extraindo tanto
 * os metadados quanto reconstruindo a árvore necessária para decodificação.
 * 
 * @param node_arena Arena de memória onde os novos nós da árvore serão alocados.
 *                   Não pode ser NULL.
 * @param buffer Buffer contendo os dados serializados (cabeçalho + árvore).
 *               Não pode ser NULL.
 * @param len Tamanho total dos dados no buffer em bytes. Deve ser suficiente
 *            para conter offset + 2 bytes de cabeçalho + árvore serializada.
 * @param offset Posição inicial no buffer (em bytes) onde a leitura começará.
 *               Geralmente 0 para ler do início do buffer.
 * 
 * @return Estrutura TreeData contendo:
 *         - trash: número de bits de padding extraído do cabeçalho
 *         - size: tamanho da árvore serializada extraído do cabeçalho
 *         - root: ponteiro para a raiz da árvore reconstruída (NULL em caso de erro)
 * 
 * @note Se root for NULL na estrutura retornada, houve erro na desserialização.
 *       Verifique sempre root antes de usar a árvore.
 * 
 * @note Os nós da árvore são alocados na arena fornecida e devem ser liberados
 *       via huffman_arena_free() quando não forem mais necessários.
 * 
 * @warning Se o buffer contiver dados corrompidos, formato inválido, ou se
 *          len for insuficiente, root será NULL ou o comportamento será indefinido.
 * 
 * @example
 * @code
 * Vector *arena = huffman_arena_new();
 * 
 * // buffer contém arquivo comprimido lido
 * struct TreeData tree_data = read_tree(arena, buffer, file_size, 0);
 * 
 * if (tree_data.root != NULL) {
 *     printf("Trash bits: %u\n", tree_data.trash);
 *     printf("Tamanho da árvore: %u bytes\n", tree_data.size);
 *     
 *     // Dados comprimidos começam em offset: 2 + tree_data.size
 *     size_t data_offset = 2 + tree_data.size;
 *     
 *     // Usar tree_data.root para decodificar
 *     Decoder decoder = new_decoder(tree_data.root);
 *     // ... decodificar dados ...
 * }
 * 
 * huffman_arena_free(arena);
 * @endcode
 * 
 * @see write_tree()
 * @see deserialize_tree()
 * @see huffman_arena_new()
 */
struct TreeData read_tree(Vector *node_arena, uint8_t *buffer, size_t len, size_t offset);

#endif // SERIALIZATION_H
