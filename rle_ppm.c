#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//-------------------------------------------------------------------------------
/*AVALIAÇÃO 2 08/07/23 | OAD 2023.1 Manoela Resende 2210100235 | COMPRESSOR
//-------------------------------------------------------------------------------

a) Com base na implementação que você fez para RLE, implemente as codificações de Huffman
e Lempel-Ziv (LZ78), e compare as taxas de compressão para os mesmos arquivos da atividade de RLE.
b) Pesquise sobre os algoritmos Lempel-Ziv-Huffman e Lempel-Ziv-Welch e compare-os.
Não há necessidade de implementá-los.*/

//-----------------
// ESTRUTURAS     <
//-----------------
typedef struct
{
    int width;
    int height;
    unsigned char *data;
} Image;

// Definição da estrutura Pixel
typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

// Definição da estrutura Dictionary
typedef struct
{
    int index;
    Pixel color;
} Dictionary;

// Definição da estrutura HuffmanNode
typedef struct HuffmanNode
{
    unsigned char symbol;
    int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

//-------------------------
// PROTOTIPO DAS FUNÇÕES  <
//-------------------------

unsigned char *rle_compressor(unsigned char *data, int size, int *compressed_size);
unsigned char *huffman_compressor(unsigned char *data, int size, int *compressed_size);
unsigned char *lz78_compressor(unsigned char *data, int size, int *compressed_size);
Image *load_ppm(const char *filename);
void save_compressed_data(const char *filename, Image *image, unsigned char *compressed_data, int compressed_size);
void process_images(const char *filenames[], int numFiles);
HuffmanNode *create_huffman_node(unsigned char symbol, int frequency, HuffmanNode *left, HuffmanNode *right);
void destroy_huffman_tree(HuffmanNode *root);
void build_frequency_table(unsigned char *data, int size, int *frequency_table);
HuffmanNode *build_huffman_tree(int *frequency_table);
void build_huffman_codes(HuffmanNode *node, char *code, char **codes);

//==============
// EXECUÇÃO    |
//==============

int main()
{
    // Lista de arquivos de imagem a serem processados
    const char *filenames[] = {
        "Image1.ppm",
        "louis.ppm",
        "magazines.ppm",
        "debbiewarhol.ppm",
        "maxresdefault.ppm",
        "EricWSchwartz.ppm"};

    // Número de arquivos de imagem
    int numFiles = sizeof(filenames) / sizeof(filenames[0]);

    // Processar as imagens
    process_images(filenames, numFiles);

    return 0;
}

//==============
// FUNÇÕES     |
//==============

// Função para comprimir dados usando o algoritmo RLE
unsigned char *rle_compressor(unsigned char *data, int size, int *compressed_size)
{
    // Alocar memória para os dados comprimidos
    unsigned char *compressed = (unsigned char *)malloc(2 * size * sizeof(unsigned char));

    // Inicializar variáveis
    unsigned char current_r = data[0];
    unsigned char current_g = data[1];
    unsigned char current_b = data[2];
    int count_rep = 1;
    int index = 0;

    // Percorrer os dados de entrada
    for (int i = 3; i < size; i += 3)
    {
        // Obter a próxima cor
        unsigned char r = data[i];
        unsigned char g = data[i + 1];
        unsigned char b = data[i + 2];

        // Verificar se a próxima cor é igual à cor atual
        if (r == current_r && g == current_g && b == current_b)
        {
            // Incrementar o contador de repetições
            count_rep++;

            // Verificar se o contador de repetições atingiu o valor máximo (256)
            if (count_rep >= 256)
            {
                // Adicionar o contador de repetições e a cor atual aos dados comprimidos
                compressed[index++] = (unsigned char)(count_rep - 1);
                compressed[index++] = current_r;
                compressed[index++] = current_g;
                compressed[index++] = current_b;

                // Reiniciar o contador de repetições
                count_rep = 1;
            }
        }
        else
        {
            // Adicionar o contador de repetições e a cor atual aos dados comprimidos
            compressed[index++] = (unsigned char)count_rep;
            compressed[index++] = current_r;
            compressed[index++] = current_g;
            compressed[index++] = current_b;

            // Atualizar a cor atual e reiniciar o contador de repetições
            current_r = r;
            current_g = g;
            current_b = b;
            count_rep = 1;
        }
    }

    // Adicionar o último contador de repetições e a última cor aos dados comprimidos
    compressed[index++] = (unsigned char)count_rep;
    compressed[index++] = current_r;
    compressed[index++] = current_g;
    compressed[index++] = current_b;

    // Armazenar o tamanho dos dados comprimidos e retornar os dados comprimidos
    *compressed_size = index;
    return compressed;
}

// Função para criar um nó da árvore de Huffman
HuffmanNode *create_huffman_node(unsigned char symbol, int frequency, HuffmanNode *left, HuffmanNode *right)
{
    // Alocar memória para o nó e inicializar seus valores
    HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    node->symbol = symbol;
    node->frequency = frequency;
    node->left = left;
    node->right = right;
    return node;
}

// Função para destruir a árvore de Huffman
void destroy_huffman_tree(HuffmanNode *root)
{
    // Verificar se o nó é nulo
    if (root == NULL)
        return;

    // Destruir os nodos filhos
    destroy_huffman_tree(root->left);
    destroy_huffman_tree(root->right);

    // Liberar a memória do nó atual
    free(root);
}

// Função para construir a tabela de frequências
void build_frequency_table(unsigned char *data, int size, int *frequency_table)
{
    // Percorrer os dados de entrada e incrementar a frequência dos símbolos
    for (int i = 0; i < size; i++)
    {
        unsigned char symbol = data[i];
        frequency_table[symbol]++;
    }
}

// Função para construir a árvore de Huffman
HuffmanNode *build_huffman_tree(int *frequency_table)
{
    // Inicializar o heap com os nodos folha da árvore de Huffman
    HuffmanNode *heap[256];
    int heap_size = 0;

    for (int i = 0; i < 256; i++)
    {
        if (frequency_table[i] > 0)
        {
            HuffmanNode *node = create_huffman_node(i, frequency_table[i], NULL, NULL);
            heap[heap_size++] = node;
        }
    }

    // Construir a árvore de Huffman combinando os dois nodos com menor frequência
    while (heap_size > 1)
    {
        // Encontrar os dois nodos com menor frequência
        int min1_index = 0;
        int min2_index = 1;

        if (heap[min1_index]->frequency > heap[min2_index]->frequency)
        {
            int temp = min1_index;
            min1_index = min2_index;
            min2_index = temp;
        }

        for (int i = 2; i < heap_size; i++)
        {
            if (heap[i]->frequency < heap[min1_index]->frequency)
            {
                min2_index = min1_index;
                min1_index = i;
            }
            else if (heap[i]->frequency < heap[min2_index]->frequency)
            {
                min2_index = i;
            }
        }

        // Combinar os dois nodos com menor frequência em um novo nó pai
        HuffmanNode *min1 = heap[min1_index];
        HuffmanNode *min2 = heap[min2_index];
        HuffmanNode *parent = create_huffman_node(0, min1->frequency + min2->frequency, min1, min2);

        // Atualizar o heap
        heap[min1_index] = parent;
        heap[min2_index] = heap[heap_size - 1];
        heap_size--;
    }

    // Retornar a raiz da árvore de Huffman
    return heap[0];
}

// Função para construir os códigos de Huffman a partir da árvore de Huffman
void build_huffman_codes(HuffmanNode *node, char *code, char **codes)
{
    // Verificar se o nó é nulo
    if (node == NULL)
        return;

    // Verificar se o nó é uma folha
    if (node->left == NULL && node->right == NULL)
    {
        // Armazenar o código de Huffman do símbolo
        int symbol = node->symbol;
        codes[symbol] = (char *)malloc(strlen(code) + 1);
        strcpy(codes[symbol], code);
        return;
    }

    // Construir os códigos de Huffman dos nodos filhos
    int code_length = strlen(code);

    // Construir o código de Huffman do nó filho à esquerda
    char *left_code = (char *)malloc(code_length + 2);
    strcpy(left_code, code);
    left_code[code_length] = '0';
    left_code[code_length + 1] = '\0';

    build_huffman_codes(node->left, left_code, codes);

    // Construir o código de Huffman do nó filho à direita
    char *right_code = (char *)malloc(code_length + 2);
    strcpy(right_code, code);
    right_code[code_length] = '1';
    right_code[code_length + 1] = '\0';

    build_huffman_codes(node->right, right_code, codes);
}

// Função para comprimir dados usando o algoritmo de Huffman
unsigned char *huffman_compressor(unsigned char *data, int size, int *compressed_size)
{
    // Construir a tabela de frequências
    int frequency_table[256] = {0};
    build_frequency_table(data, size, frequency_table);

    // Construir a árvore de Huffman
    HuffmanNode *huffman_tree = build_huffman_tree(frequency_table);

    // Construir os códigos de Huffman
    char *codes[256] = {NULL};
    build_huffman_codes(huffman_tree, "", codes);

    // Encontrar o comprimento máximo do código de Huffman
    int max_code_length = 0;
    for (int i = 0; i < 256; i++)
    {
        if (codes[i] != NULL && strlen(codes[i]) > max_code_length)
            max_code_length = strlen(codes[i]);
    }

    // Calcular o tamanho máximo dos dados comprimidos
    int max_compressed_size = (size * max_code_length) / 8 + 1;

    // Alocar memória para os dados comprimidos
    unsigned char *compressed_data = (unsigned char *)malloc(max_compressed_size * sizeof(unsigned char));
    int compressed_index = 0;

    // Inicializar variáveis
    unsigned char current_byte = 0;
    int current_bit = 0;

    // Percorrer os dados de entrada e construir os dados comprimidos
    for (int i = 0; i < size; i++)
    {
        unsigned char symbol = data[i];
        char *code = codes[symbol];
        int code_length = strlen(code);

        for (int j = 0; j < code_length; j++)
        {
            // Verificar se o byte atual está cheio
            if (current_bit == 8)
            {
                // Adicionar o byte atual aos dados comprimidos e reiniciar o byte e o contador de bits
                compressed_data[compressed_index++] = current_byte;
                current_byte = 0;
                current_bit = 0;
            }

            // Adicionar o bit atual ao byte atual
            if (code[j] == '1')
                current_byte |= (1 << current_bit);

            current_bit++;
        }
    }

    // Adicionar o último byte aos dados comprimidos, se necessário
    if (current_bit > 0)
        compressed_data[compressed_index++] = current_byte;

    // Armazenar o tamanho dos dados comprimidos e retornar os dados comprimidos
    *compressed_size = compressed_index;

    // Liberar a memória da árvore de Huffman e dos códigos de Huffman
    destroy_huffman_tree(huffman_tree);

    for (int i = 0; i < 256; i++)
    {
        if (codes[i] != NULL)
            free(codes[i]);
    }

    return compressed_data;
}

// função que comprime com o algoritmo de LZ78
unsigned char *lz78_compressor(unsigned char *data, int size, int *compressed_size)
{
    // Definir o tamanho máximo do dicionário
    int max_dict_size = 256;

    // Inicializar o dicionário
    Dictionary dictionary[max_dict_size];
    int dict_size = 0;

    // Alocar memória para os dados comprimidos
    unsigned char *compressed = (unsigned char *)malloc(2 * size * sizeof(unsigned char));
    int index = 0;

    // Inicializar a cor atual com a primeira cor dos dados de entrada
    Pixel current_color;
    current_color.r = data[0];
    current_color.g = data[1];
    current_color.b = data[2];

    // Percorrer os dados de entrada
    int i;
    for (i = 3; i < size; i += 3)
    {
        // Obter a próxima cor dos dados de entrada
        Pixel next_color;
        next_color.r = data[i];
        next_color.g = data[i + 1];
        next_color.b = data[i + 2];

        // Verificar se a próxima cor está no dicionário
        int j;
        for (j = 0; j < dict_size; j++)
        {
            if (dictionary[j].color.r == next_color.r && dictionary[j].color.g == next_color.g && dictionary[j].color.b == next_color.b)
                break;
        }

        // Se a próxima cor não está no dicionário
        if (j == dict_size)
        {
            // Adicionar o índice e a cor atual aos dados comprimidos
            compressed[index++] = dict_size;
            compressed[index++] = current_color.r;
            compressed[index++] = current_color.g;
            compressed[index++] = current_color.b;

            // Adicionar a próxima cor ao dicionário, se houver espaço
            if (dict_size < max_dict_size)
            {
                dictionary[dict_size].index = dict_size;
                dictionary[dict_size].color = next_color;
                dict_size++;
            }

            // Atualizar a cor atual
            current_color = next_color;
        }
        else
        {
            // Atualizar a cor atual
            current_color = next_color;
        }
    }

    // Adicionar o último índice e a última cor aos dsdos comprimidos
    compressed[index++] = dict_size;
    compressed[index++] = current_color.r;
    compressed[index++] = current_color.g;
    compressed[index++] = current_color.b;

    // Armazenar o tamanho dos dados comprimidos e retornar os dados comprimidos
    *compressed_size = index;
    return compressed;
}

// Função para carregar uma imagem PPM a partir de um arquivo
Image *load_ppm(const char *filename)
{
    // Abrir o arquivo para leitura
    FILE *arquivo = fopen(filename, "rb");
    // Verificar se o arquivo foi aberto com sucesso
    if (!arquivo)
    {
        fprintf(stderr, "Falha ao abrir o arquivo: %s\n", filename);
        return NULL;
    }

    // Ler o formato da imagem
    char formato[3];
    fscanf(arquivo, "%2s\n", formato);

    // Verificar se o formato é válido
    if (formato[0] != 'P' || formato[1] != '6')
    {
        fprintf(stderr, "Formato PPM inválido: %s\n", formato);
        fclose(arquivo);
        return NULL;
    }

    // Alocar memória para a estrutura Image
    Image *image = malloc(sizeof(Image));

    // Le o cabeçalho da imagem
    int c;
    while ((c = fgetc(arquivo)) == '#')
    {
        while (fgetc(arquivo) != '\n')
            ;
    }
    ungetc(c, arquivo);
    fscanf(arquivo, "%d %d\n", &image->width, &image->height);
    int valor_maximo;
    fscanf(arquivo, "%d\n", &valor_maximo);

    // Alocar memória para os dados da imagem e ler os dados da imagem
    image->data = malloc(image->width * image->height * 3);
    fread(image->data, 3, image->width * image->height, arquivo);

    // Fechar o arquivo e retornar a imagem
    fclose(arquivo);
    return image;
}

void save_compressed_data(const char *filename, Image *image, unsigned char *compressed_data, int compressed_size)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        fprintf(stderr, "Erro ao abrir o arquivo %s para escrita\n", filename);
        return;
    }

    // Escrever o cabeçalho PPM no arquivo
    fprintf(file, "P6\n");
    fprintf(file, "%d %d\n", image->width, image->height);
    fprintf(file, "255\n");

    // Escrever os dados comprimidos no arquivo
    fwrite(compressed_data, sizeof(unsigned char), compressed_size, file);
    fclose(file);
}

// Função para processar várias imagens
void process_images(const char *filenames[], int numFiles)
{
    // Abrir o arquivo para escrita
    FILE *file = fopen("comparacoes_compressores.txt", "w");
    if (!file)
    {
        fprintf(stderr, "Erro ao abrir o arquivo comparacoes_compressores.txt para escrita\n");
        return;
    }

    // Percorrer a lista de arquivos de imagemm
    for (int i = 0; i < numFiles; i++)
    {
        const char *nomeArquivo = filenames[i];

        // Carregar a imagem do arquivo
        Image *image = load_ppm(nomeArquivo);
        if (image)
        {
            fprintf(file, "\nImagem: %s\n", nomeArquivo);
            fprintf(file, "Dimensoes: %dx%d\n", image->width, image->height);

            // Calcular o tamanho original dos dados da imagem
            int tamanho_original = image->width * image->height * 3;
            int compressed_size;
            unsigned char *dado_comprimido;
            double taxa_compressao;

            fprintf(file, "Tamanho original: %d bytes\n", tamanho_original);

            // RLE
            dado_comprimido = rle_compressor(image->data, tamanho_original, &compressed_size);
            taxa_compressao = (1.0 - compressed_size / (double)tamanho_original) * 100;
            fprintf(file, "\n------Metodo: RLE\n");
            fprintf(file, "Tamanho compressed: %d bytes\n", compressed_size);
            fprintf(file, "Taxa de compressao: %.2f%%\n", taxa_compressao);

            // Salvar dados comprimidos em um arquivo
            char output_filename[256];
            sprintf(output_filename, "compressoes/RLE/RLE-%s", nomeArquivo);
            save_compressed_data(output_filename, image, dado_comprimido, compressed_size);

            // Huffman
            dado_comprimido = huffman_compressor(image->data, tamanho_original, &compressed_size);
            taxa_compressao = (1.0 - compressed_size / (double)tamanho_original) * 100;
            fprintf(file, "\n------Metodo: Huffman\n");
            fprintf(file, "Tamanho compressed: %d bytes\n", compressed_size);
            fprintf(file, "Taxa de compressao: %.2f%%\n", taxa_compressao);

            sprintf(output_filename, "compressoes/HUFFMAN/HUFF-%s", nomeArquivo);
            save_compressed_data(output_filename, image, dado_comprimido, compressed_size);

            // LZ78
            dado_comprimido = lz78_compressor(image->data, tamanho_original, &compressed_size);
            taxa_compressao = (1.0 - compressed_size / (double)tamanho_original) * 100;
            fprintf(file, "\n------Metodo: LZ78\n");
            fprintf(file, "Tamanho compressed: %d bytes\n", compressed_size);
            fprintf(file, "Taxa de compressao: %.2f%%\n", taxa_compressao);

            sprintf(output_filename, "compressoes/LZ78/LZ78-%s", nomeArquivo);
            save_compressed_data(output_filename, image, dado_comprimido, compressed_size);

            fprintf(file, "----------------------------------------------------------------\n");

            // Liberar a memória da imagem
            free(image->data);
            free(image);
        }
    }
    // Fechar o arquivo
    fclose(file);
}
