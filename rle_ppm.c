#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct
{
    int largura;
    int altura;
    unsigned char *dados;
} Imagem;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

typedef struct
{
    int indice;
    Pixel cor;
} Dicionario;

unsigned char *rle_compressor(unsigned char *dado, int size, int *tam_comprimido)
{
    unsigned char *comprimido = (unsigned char *)malloc(2 * size * sizeof(unsigned char));

    unsigned char atual_r = dado[0];
    unsigned char atual_g = dado[1];
    unsigned char atual_b = dado[2];
    int count_rep = 1;
    int index = 0;

    for (int i = 3; i < size; i += 3)
    {
        unsigned char r = dado[i];
        unsigned char g = dado[i + 1];
        unsigned char b = dado[i + 2];

        if (r == atual_r && g == atual_g && b == atual_b)
        {
            count_rep++;

            if (count_rep >= 256)
            {
                comprimido[index++] = (unsigned char)(count_rep - 1);
                comprimido[index++] = atual_r;
                comprimido[index++] = atual_g;
                comprimido[index++] = atual_b;
                count_rep = 1;
            }
        }
        else
        {
            comprimido[index++] = (unsigned char)count_rep;
            comprimido[index++] = atual_r;
            comprimido[index++] = atual_g;
            comprimido[index++] = atual_b;
            atual_r = r;
            atual_g = g;
            atual_b = b;
            count_rep = 1;
        }
    }

    comprimido[index++] = (unsigned char)count_rep;
    comprimido[index++] = atual_r;
    comprimido[index++] = atual_g;
    comprimido[index++] = atual_b;
    *tam_comprimido = index;
    return comprimido;
}

typedef struct HuffmanNode
{
    unsigned char symbol;
    int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

HuffmanNode *create_huffman_node(unsigned char symbol, int frequency, HuffmanNode *left, HuffmanNode *right)
{
    HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    node->symbol = symbol;
    node->frequency = frequency;
    node->left = left;
    node->right = right;
    return node;
}

void destroy_huffman_tree(HuffmanNode *root)
{
    if (root == NULL)
        return;

    destroy_huffman_tree(root->left);
    destroy_huffman_tree(root->right);
    free(root);
}

void build_frequency_table(unsigned char *data, int size, int *frequency_table)
{
    for (int i = 0; i < size; i++)
    {
        unsigned char symbol = data[i];
        frequency_table[symbol]++;
    }
}

HuffmanNode *build_huffman_tree(int *frequency_table)
{
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

    while (heap_size > 1)
    {
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

        HuffmanNode *min1 = heap[min1_index];
        HuffmanNode *min2 = heap[min2_index];
        HuffmanNode *parent = create_huffman_node(0, min1->frequency + min2->frequency, min1, min2);

        heap[min1_index] = parent;
        heap[min2_index] = heap[heap_size - 1];
        heap_size--;
    }

    return heap[0];
}

void build_huffman_codes(HuffmanNode *node, char *code, char **codes)
{
    if (node == NULL)
        return;

    if (node->left == NULL && node->right == NULL)
    {
        int symbol = node->symbol;
        codes[symbol] = (char *)malloc(strlen(code) + 1);
        strcpy(codes[symbol], code);
        return;
    }

    int code_length = strlen(code);
    char *left_code = (char *)malloc(code_length + 2);
    strcpy(left_code, code);
    left_code[code_length] = '0';
    left_code[code_length + 1] = '\0';

    build_huffman_codes(node->left, left_code, codes);

    char *right_code = (char *)malloc(code_length + 2);
    strcpy(right_code, code);
    right_code[code_length] = '1';
    right_code[code_length + 1] = '\0';

    build_huffman_codes(node->right, right_code, codes);
}

unsigned char *huffman_compressor(unsigned char *data, int size, int *compressed_size)
{
    int frequency_table[256] = {0};
    build_frequency_table(data, size, frequency_table);

    HuffmanNode *huffman_tree = build_huffman_tree(frequency_table);

    char *codes[256] = {NULL};
    build_huffman_codes(huffman_tree, "", codes);

    int max_code_length = 0;
    for (int i = 0; i < 256; i++)
    {
        if (codes[i] != NULL && strlen(codes[i]) > max_code_length)
            max_code_length = strlen(codes[i]);
    }

    int max_compressed_size = (size * max_code_length) / 8 + 1;
    unsigned char *compressed_data = (unsigned char *)malloc(max_compressed_size * sizeof(unsigned char));
    int compressed_index = 0;

    unsigned char current_byte = 0;
    int current_bit = 0;

    for (int i = 0; i < size; i++)
    {
        unsigned char symbol = data[i];
        char *code = codes[symbol];
        int code_length = strlen(code);

        for (int j = 0; j < code_length; j++)
        {
            if (current_bit == 8)
            {
                compressed_data[compressed_index++] = current_byte;
                current_byte = 0;
                current_bit = 0;
            }

            if (code[j] == '1')
                current_byte |= (1 << current_bit);

            current_bit++;
        }
    }

    if (current_bit > 0)
        compressed_data[compressed_index++] = current_byte;

    *compressed_size = compressed_index;
    destroy_huffman_tree(huffman_tree);

    for (int i = 0; i < 256; i++)
    {
        if (codes[i] != NULL)
            free(codes[i]);
    }

    return compressed_data;
}

unsigned char *lz78_compressor(unsigned char *dado, int size, int *tam_comprimido)
{
    int max_dicionario_size = 256; // ou qualquer outro valor que você desejar
    Dicionario *dicionario = (Dicionario *)malloc(max_dicionario_size * sizeof(Dicionario));

    int dicionario_size = 0;

    unsigned char *comprimido = (unsigned char *)malloc(2 * size * sizeof(unsigned char));
    int index = 0;

    for (int i = 0; i < size; i += 3)
    {
        Pixel cor;
        cor.r = dado[i];
        cor.g = dado[i + 1];
        cor.b = dado[i + 2];

        int j = i + 3;
        while (j < size)
        {
            Pixel proxima_cor;
            proxima_cor.r = dado[j];
            proxima_cor.g = dado[j + 1];
            proxima_cor.b = dado[j + 2];

            int k;
            for (k = 0; k < dicionario_size; k++)
            {
                if (dicionario[k].cor.r == proxima_cor.r && dicionario[k].cor.g == proxima_cor.g && dicionario[k].cor.b == proxima_cor.b)
                    break;
            }

            if (k == dicionario_size)
            {
                comprimido[index++] = dicionario_size;
                comprimido[index++] = cor.r;
                comprimido[index++] = cor.g;
                comprimido[index++] = cor.b;

                if (dicionario_size < max_dicionario_size)
                {
                    dicionario[dicionario_size].cor = proxima_cor;
                    dicionario_size++;
                }

                break;
            }

            cor = proxima_cor;
            j += 3;
        }
    }

    *tam_comprimido = index;
    free(dicionario);
    return comprimido;
}

Imagem *carregar_ppm(const char *nome_arquivo)
{
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo)
    {
        fprintf(stderr, "Falha ao abrir o arquivo: %s\n", nome_arquivo);
        return NULL;
    }

    char formato[3];
    fscanf(arquivo, "%2s\n", formato);

    if (formato[0] != 'P' || formato[1] != '6')
    {
        fprintf(stderr, "Formato PPM inválido: %s\n", formato);
        fclose(arquivo);
        return NULL;
    }

    Imagem *imagem = malloc(sizeof(Imagem));
    int c;
    while ((c = fgetc(arquivo)) == '#')
    {
        while (fgetc(arquivo) != '\n')
            ;
    }
    ungetc(c, arquivo);
    fscanf(arquivo, "%d %d\n", &imagem->largura, &imagem->altura);
    int valor_maximo;
    fscanf(arquivo, "%d\n", &valor_maximo);
    imagem->dados = malloc(imagem->largura * imagem->altura * 3);
    fread(imagem->dados, 3, imagem->largura * imagem->altura, arquivo);
    fclose(arquivo);
    return imagem;
}

void processar_imagens(const char *nomesArquivos[], int numArquivos)
{
    for (int i = 0; i < numArquivos; i++)
    {
        const char *nomeArquivo = nomesArquivos[i];
        Imagem *imagem = carregar_ppm(nomeArquivo);
        if (imagem)
        {
            printf("\nImagem: %s\n", nomeArquivo);
            printf("Dimensoes: %dx%d\n", imagem->largura, imagem->altura);

            int tamanho_original = imagem->largura * imagem->altura * 3;
            int tam_comprimido;
            unsigned char *dado_comprimido;
            double taxa_compressao;

            printf("Tamanho original: %d bytes\n", tamanho_original);
            // RLE
            dado_comprimido = rle_compressor(imagem->dados, tamanho_original, &tam_comprimido);
            taxa_compressao = (1.0 - tam_comprimido / (double)tamanho_original) * 100;
            printf("\n------Metodo: RLE\n");
            printf("Tamanho comprimido: %d bytes\n", tam_comprimido);
            printf("Taxa de compressao: %.2f%%\n", taxa_compressao);

            // Huffman
            dado_comprimido = huffman_compressor(imagem->dados, tamanho_original, &tam_comprimido);
            taxa_compressao = (1.0 - tam_comprimido / (double)tamanho_original) * 100;
            printf("\n------Metodo: Huffman\n");
            //printf("Tamanho original: %d bytes\n", tamanho_original);
            printf("Tamanho comprimido: %d bytes\n", tam_comprimido);
            printf("Taxa de compressao: %.2f%%\n", taxa_compressao);

            // LZ78
            dado_comprimido = lz78_compressor(imagem->dados, tamanho_original, &tam_comprimido);
            taxa_compressao = (1.0 - tam_comprimido / (double)tamanho_original) * 100;
            printf("\n------Metodo: LZ78\n");
            //printf("Tamanho original: %d bytes\n", tamanho_original);
            printf("Tamanho comprimido: %d bytes\n", tam_comprimido);
            printf("Taxa de compressao: %.2f%%\n", taxa_compressao);

            printf("----------------------------------------------------------------\n");
            free(imagem->dados);
            free(imagem);
        }
    }
}


int main()
{
   const char *nomesArquivos[] = {
    "Image1.ppm",
    "louis.ppm",
    "magazines.ppm",
    "debbiewarhol.ppm",
    "maxresdefault.ppm",
    "EricWSchwartz.ppm"
};

int numArquivos = sizeof(nomesArquivos) / sizeof(nomesArquivos[0]);

processar_imagens(nomesArquivos, numArquivos);

    return 0;
}
