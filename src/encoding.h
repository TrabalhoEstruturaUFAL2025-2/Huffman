#ifndef ENCODER_H
#define ENCODER_H

#include "Huffman.h"
#include "bin_utils.h"

#include <string.h>

/**
 * @struct Codeword
 * @brief Estrutura POD (Plain Old Data) que representa uma palavra-código de Huffman.
 * 
 * @details Armazena a sequência de bits que representa a codificação de um símbolo
 * específico na árvore de Huffman. Limitada a 8 bits de comprimento, adequada para
 * árvores de Huffman pequenas e otimizada para uso de memória.
 */
struct Codeword {
    /**
     * @brief Bits que compõem a palavra-código.
     * @details Armazena até 8 bits da palavra-código em um único byte.
     * Os bits são lidos/escritos de acordo com a convenção definida em bin_utils.h.
     */
    uint8_t bits;
    
    /**
     * @brief Comprimento da palavra-código em bits.
     * @details Indica quantos bits são válidos em 'bits'. Varia de 0 (não inicializado)
     * até 8 (palavra-código completa de 8 bits).
     */
    int length;
};

/**
 * @struct Encoder
 * @brief Estrutura responsável por codificar bytes usando a árvore de Huffman.
 * 
 * @details Mantém uma tabela de lookup que mapeia cada byte possível (0-255)
 * para sua palavra-código correspondente. Esta abordagem oferece codificação
 * em tempo O(1) após a inicialização.
 */
typedef struct Encoder {
    /**
     * @brief Tabela de mapeamento byte -> palavra-código.
     * @details Array de 256 posições onde cada índice representa um byte
     * e cada elemento contém a Codeword correspondente. Apenas símbolos
     * presentes no texto original terão Codewords válidas (length > 0).
     */
    struct Codeword table[256];
} Encoder;

/**
 * @brief Inicializa o encoder percorrendo a árvore de Huffman.
 * 
 * @details Constrói a tabela de codificação percorrendo a árvore de Huffman
 * a partir da raiz. Para cada nó folha encontrado, calcula e armazena a
 * palavra-código correspondente na tabela. A palavra-código é construída
 * registrando o caminho da raiz até a folha (0 para esquerda, 1 para direita).
 * 
 * O algoritmo usa busca em profundidade (DFS) para percorrer a árvore,
 * acumulando bits no caminho até encontrar folhas.
 * 
 * @param encoder Ponteiro para a estrutura Encoder a ser inicializada.
 *                Não pode ser NULL. A tabela será zerada e preenchida com
 *                as palavras-código da árvore.
 * @param root Ponteiro para o nó raiz da árvore de Huffman construída.
 *             Não pode ser NULL.
 * 
 * @note Após a inicialização, apenas os índices correspondentes aos símbolos
 *       presentes na árvore terão Codewords válidas (length > 0).
 * 
 * @note A complexidade é O(N) onde N é o número de nós na árvore.
 * 
 * @note Esta função não retorna valor. Certifique-se de que os parâmetros
 *       sejam válidos antes de chamar.
 * 
 * @warning Palavras-código com mais de 8 bits não podem ser armazenadas
 *          nesta implementação e serão truncadas ou causarão comportamento
 *          indefinido.
 * 
 * @example
 * @code
 * HuffmanNode *root = huffman_build_tree(arena);
 * Encoder encoder;
 * init_encoder(&encoder, root);
 * // Encoder pronto para uso
 * @endcode
 * 
 * @see huffman_build_tree()
 * @see encoder_get_codeword()
 */
void init_encoder(Encoder *encoder, HuffmanNode *root);

/**
 * @brief Obtém a palavra-código correspondente a um byte.
 * 
 * @details Realiza lookup O(1) na tabela de codificação para retornar
 * a palavra-código do byte especificado. Esta função é usada durante
 * o processo de compressão para converter bytes originais em códigos.
 * 
 * @param encoder Ponteiro para o Encoder inicializado. Não pode ser NULL.
 * @param byte Byte (0-255) cuja palavra-código será retornada.
 * 
 * @return Ponteiro para a Codeword correspondente ao byte, ou NULL se
 *         o encoder for NULL. Se o byte não possuir codificação válida
 *         (símbolo não presente no texto original), a Codeword retornada
 *         terá length == 0.
 * 
 * @note O ponteiro retornado aponta para dados internos do encoder e
 *       não deve ser liberado pelo chamador. O conteúdo pode ser lido
 *       mas não deve ser modificado.
 * 
 * @note Sempre verifique se codeword->length > 0 antes de usar a
 *       palavra-código retornada.
 * 
 * @example
 * @code
 * struct Codeword *cw = encoder_get_codeword(&encoder, 'A');
 * if (cw != NULL && cw->length > 0) {
 *     // Usar a palavra-código
 *     encoder_write(&encoder, cw, buffer, buffer_size, offset);
 * }
 * @endcode
 * 
 * @see init_encoder()
 * @see encoder_write()
 */
struct Codeword *encoder_get_codeword(Encoder *encoder, uint8_t byte);

/**
 * @brief Escreve uma palavra-código em um buffer a partir de um offset de bits.
 * 
 * @details Escreve os bits da palavra-código no buffer começando na posição
 * especificada pelo offset em bits. Esta função permite escrever múltiplas
 * palavras-código de forma contínua em um buffer, preenchendo os bytes bit a bit.
 * 
 * Por exemplo, com um buffer de 4 bytes (32 bits) e bit_offset=5, a escrita
 * começará no 6º bit (índice 5) do buffer.
 * 
 * A função utiliza as operações de bit definidas em bin_utils.h para manipular
 * os bits individuais no buffer.
 * 
 * @param encoder Ponteiro para a estrutura Encoder. Não pode ser NULL.
 * @param codeword Ponteiro para a Codeword a ser escrita. Não pode ser NULL
 *                 e deve ter length > 0.
 * @param buffer Buffer de destino onde os bits serão escritos. Não pode ser NULL.
 * @param buffer_size Tamanho do buffer em bytes. Deve ser suficiente para
 *                    acomodar a escrita considerando o offset.
 * @param bit_offset Offset em bits onde a escrita começará. Deve ser >= 0
 *                   e < (buffer_size * 8).
 * 
 * @return Novo offset em bits após a escrita (bit_offset + codeword->length),
 *         permitindo escrita sequencial de múltiplas palavras-código.
 *         Retorna valor negativo em caso de erro (parâmetros inválidos,
 *         buffer insuficiente, ou overflow).
 * 
 * @note A função não limpa os bits existentes no buffer antes de escrever.
 *       Certifique-se de que o buffer esteja zerado se necessário.
 * 
 * @note A ordem de escrita dos bits (MSB first ou LSB first) é definida
 *       pela implementação em bin_utils.h.
 * 
 * @example
 * @code
 * uint8_t buffer[1024] = {0};
 * int offset = 0;
 * Encoder encoder;
 * init_encoder(&encoder, root);
 * 
 * // Escrever múltiplas palavras-código
 * for (size_t i = 0; i < data_size; i++) {
 *     struct Codeword *cw = encoder_get_codeword(&encoder, data[i]);
 *     if (cw != NULL && cw->length > 0) {
 *         offset = encoder_write(&encoder, cw, buffer, sizeof(buffer), offset);
 *         if (offset < 0) {
 *             // Erro ou buffer cheio
 *             break;
 *         }
 *     }
 * }
 * 
 * // Calcular bytes escritos
 * int bytes_written = (offset + 7) / 8; // Arredondar para cima
 * @endcode
 * 
 * @warning Certifique-se de que o buffer tenha espaço suficiente para
 *          acomodar todos os bits. A função pode retornar erro se houver
 *          tentativa de escrever além do limite do buffer.
 * 
 * @see encoder_get_codeword()
 * @see init_encoder()
 */
int encoder_write(
    Encoder *encoder, 
    struct Codeword *codeword, 
    uint8_t *buffer, 
    size_t buffer_size,
    int bit_offset
);

/**
 * @struct Decoder
 * @brief Máquina de estados para decodificar bytes comprimidos com Huffman.
 * 
 * @details Funciona como um autômato que consome bits sequencialmente,
 * navegando pela árvore de Huffman até encontrar símbolos (nós folha).
 * Mantém estado entre chamadas para processar streams de bytes de forma
 * incremental.
 */
typedef struct Decoder {
    /**
     * @brief Byte atualmente sendo processado.
     * @details Contém o byte fornecido pela última chamada bem-sucedida
     * de decoder_feed().
     */
    uint8_t byte;
    
    /**
     * @brief Índice do bit atual sendo processado no byte.
     * @details Varia de 0 a 7, indicando qual bit do byte será
     * lido na próxima iteração. Quando atinge 8, o byte foi completamente
     * processado e um novo byte deve ser alimentado.
     */
    int curr_bit;
    
    /**
     * @brief Ponteiro para a raiz da árvore de Huffman.
     * @details Usado para reiniciar a navegação após encontrar um símbolo.
     * Permanece constante durante toda a vida do decoder.
     */
    HuffmanNode *root;
    
    /**
     * @brief Ponteiro para o nó atual na navegação da árvore.
     * @details Atualizado a cada bit consumido, movendo-se para o filho
     * esquerdo (bit 0) ou direito (bit 1). Quando alcança uma folha,
     * o símbolo é decodificado e current_node é resetado para root.
     */
    HuffmanNode *current_node;
} Decoder;

/**
 * @brief Cria e inicializa um novo decoder com a árvore de Huffman.
 * 
 * @details Cria uma estrutura Decoder configurada para começar a decodificar
 * dados comprimidos. Configura o estado inicial do decoder, definindo a árvore
 * a ser usada e preparando os contadores de posição.
 * 
 * @param root Ponteiro para o nó raiz da árvore de Huffman. Não pode ser NULL.
 * 
 * @return Estrutura Decoder inicializada e pronta para uso. O decoder retornado
 *         terá:
 *         - root e current_node apontando para o parâmetro root
 *         - curr_bit definido como 8 (indicando necessidade de alimentar um byte)
 *         - byte indefinido até o primeiro decoder_feed()
 * 
 * @note Esta função retorna a estrutura por valor, não por ponteiro.
 * 
 * @note Se root for NULL, o comportamento é indefinido. Certifique-se de
 *       passar uma árvore válida.
 * 
 * @example
 * @code
 * HuffmanNode *root = huffman_build_tree(arena);
 * Decoder decoder = new_decoder(root);
 * // Decoder pronto para uso
 * @endcode
 * 
 * @see decoder_feed()
 * @see decoder_consume()
 * @see huffman_build_tree()
 */
Decoder new_decoder(HuffmanNode *root);

/**
 * @brief Alimenta um byte para o decoder processar.
 * 
 * @details Esta função gerencia o estado do decoder, fornecendo novos bytes
 * para processamento. Se ainda houver bits não processados do byte anterior
 * (curr_bit < 8), a função retorna -1 indicando que o byte anterior deve ser
 * completamente consumido antes de aceitar um novo.
 * 
 * Quando o decoder está pronto (curr_bit >= 8), esta função aceita o novo byte,
 * armazena em decoder->byte, reseta curr_bit para 0 e retorna 0.
 * 
 * @param decoder Ponteiro para a estrutura Decoder. Não pode ser NULL.
 * @param byte Byte a ser processado pelo decoder.
 * 
 * @return 0 se o byte foi aceito e armazenado (decoder estava pronto).
 *         -1 se o decoder ainda está processando o byte anterior (curr_bit < 8).
 * 
 * @note Esta função não processa os bits, apenas prepara o decoder. Use
 *       decoder_consume() para processar os bits do byte.
 * 
 * @note Após retornar 0, curr_bit é resetado para 0 e o decoder está pronto
 *       para começar a processar o novo byte.
 * 
 * @example
 * @code
 * Decoder decoder = new_decoder(root);
 * uint8_t output[1024];
 * size_t output_pos = 0;
 * 
 * for (size_t i = 0; i < compressed_size; i++) {
 *     // Tentar alimentar novo byte
 *     while (decoder_feed(&decoder, compressed_data[i]) == -1) {
 *         // Consumir byte anterior completamente
 *         uint8_t symbol;
 *         if (decoder_consume(&decoder, &symbol) == 1) {
 *             output[output_pos++] = symbol;
 *         }
 *     }
 *     
 *     // Processar o byte recém alimentado
 *     uint8_t symbol;
 *     while (decoder_consume(&decoder, &symbol) == 1) {
 *         output[output_pos++] = symbol;
 *     }
 * }
 * @endcode
 * 
 * @see decoder_consume()
 * @see new_decoder()
 */
int decoder_feed(Decoder *decoder, uint8_t byte);

/**
 * @brief Consome bits do byte atual, navegando pela árvore até encontrar um símbolo.
 * 
 * @details Itera pelos bits do byte atual começando de curr_bit, navegando
 * pela árvore de Huffman: bit 0 move para o filho esquerdo, bit 1 move para
 * o filho direito. Quando alcança um nó folha, escreve o símbolo em 'out',
 * reseta current_node para root, e retorna 1.
 * 
 * Se todos os bits do byte forem consumidos sem encontrar uma folha (curr_bit
 * atinge 8), retorna 0 indicando que mais bits são necessários (próximo byte).
 * 
 * A ordem de leitura dos bits (MSB first ou LSB first) é definida pela
 * implementação em bin_utils.h.
 * 
 * @param decoder Ponteiro para a estrutura Decoder. Não pode ser NULL.
 * @param out Ponteiro onde o símbolo decodificado será escrito quando
 *            uma folha for encontrada. Não pode ser NULL.
 * 
 * @return 1 se um símbolo foi decodificado e escrito em 'out'.
 *         0 se o byte foi completamente consumido sem completar uma palavra-código
 *           (curr_bit alcançou 8, mas ainda em nó interno).
 *         Valor negativo em caso de erro (decoder NULL, out NULL, ou estado
 *         inválido do decoder).
 * 
 * @note Esta função atualiza curr_bit a cada iteração. Quando retorna 0,
 *       curr_bit estará em 8, indicando que o byte foi completamente
 *       processado e um novo byte deve ser alimentado com decoder_feed().
 * 
 * @note Esta função pode ser chamada múltiplas vezes no mesmo byte se
 *       múltiplos símbolos curtos estiverem codificados no mesmo byte.
 * 
 * @note Após retornar 1, current_node é resetado para root, mas curr_bit
 *       mantém sua posição para continuar processando o byte atual se
 *       houver bits restantes.
 * 
 * @example
 * @code
 * Decoder decoder = new_decoder(root);
 * decoder_feed(&decoder, compressed_byte);
 * 
 * uint8_t symbol;
 * int result;
 * 
 * // Processar todos os símbolos possíveis no byte atual
 * while ((result = decoder_consume(&decoder, &symbol)) == 1) {
 *     printf("Símbolo decodificado: 0x%02X\n", symbol);
 *     output[pos++] = symbol;
 * }
 * 
 * if (result == 0) {
 *     // Byte completamente consumido, precisa de mais dados
 *     // Alimentar próximo byte com decoder_feed()
 * } else if (result < 0) {
 *     // Erro durante decodificação
 *     fprintf(stderr, "Erro ao decodificar\n");
 * }
 * @endcode
 * 
 * @warning Se a árvore de Huffman for inválida ou corrompida, a navegação
 *          pode levar a comportamento indefinido. Certifique-se de que
 *          root aponta para uma árvore válida.
 * 
 * @warning Se encontrar um nó interno sem filhos ou tentar navegar além
 *          das folhas, o comportamento é indefinido.
 * 
 * @see decoder_feed()
 * @see new_decoder()
 */
int decoder_consume(Decoder *decoder, uint8_t *out);

#endif // ENCODER_H
