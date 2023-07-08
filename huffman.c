#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Struct para representar os nós da árvore de Huffman
typedef struct HuffmanNode
{
    unsigned char symbol;
    int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

// Struct para representar a tabela de frequências de Huffman
typedef struct
{
    int size;
    HuffmanNode **nodes;
} HuffmanTable;

// Função auxiliar para criar um nó da árvore de Huffman
HuffmanNode *createHuffmanNode(unsigned char symbol, int frequency, HuffmanNode *left, HuffmanNode *right)
{
    HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    node->symbol = symbol;
    node->frequency = frequency;
    node->left = left;
    node->right = right;
    return node;
}

// Função auxiliar para criar uma tabela de frequências de Huffman
HuffmanTable *createHuffmanTable(int size)
{
    HuffmanTable *table = (HuffmanTable *)malloc(sizeof(HuffmanTable));
    table->size = size;
    table->nodes = (HuffmanNode **)malloc(size * sizeof(HuffmanNode *));
    return table;
}

// Função auxiliar para liberar a memória alocada para a árvore de Huffman
void freeHuffmanTree(HuffmanNode *node)
{
    if (node != NULL)
    {
        freeHuffmanTree(node->left);
        freeHuffmanTree(node->right);
        free(node);
    }
}

// Função auxiliar para liberar a memória alocada para a tabela de Huffman
void freeHuffmanTable(HuffmanTable *table)
{
    if (table != NULL)
    {
        for (int i = 0; i < table->size; i++)
        {
            freeHuffmanTree(table->nodes[i]);
        }
        free(table->nodes);
        free(table);
    }
}

// Função auxiliar para construir a tabela de frequências de Huffman
HuffmanTable *buildHuffmanTable(unsigned char *data, int size)
{
    int frequency[256] = {0};

    for (int i = 0; i < size; i++)
    {
        frequency[data[i]]++;
    }

    HuffmanTable *table = createHuffmanTable(256);

    int nodeCount = 0;
    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            HuffmanNode *node = createHuffmanNode(i, frequency[i], NULL, NULL);
            table->nodes[nodeCount] = node;
            nodeCount++;
        }
    }

    table->size = nodeCount;

    return table;
}

// Função auxiliar para comparar os nós da árvore de Huffman durante a construção
int compareHuffmanNodes(const void *a, const void *b)
{
    HuffmanNode *nodeA = *(HuffmanNode **)a;
    HuffmanNode *nodeB = *(HuffmanNode **)b;

    return nodeA->frequency - nodeB->frequency;
}

// Função auxiliar para construir a árvore de Huffman
HuffmanNode *buildHuffmanTree(HuffmanTable *table)
{
    while (table->size > 1)
    {
        qsort(table->nodes, table->size, sizeof(HuffmanNode *), compareHuffmanNodes);

        HuffmanNode *left = table->nodes[0];
        HuffmanNode *right = table->nodes[1];

        HuffmanNode *parent = createHuffmanNode(0, left->frequency + right->frequency, left, right);

        table->nodes[0] = parent;
        for (int i = 1; i < table->size - 1; i++)
        {
            table->nodes[i] = table->nodes[i + 1];
        }
        table->size--;
    }

    return table->nodes[0];
}

// Função auxiliar para percorrer a árvore de Huffman e construir as codificações
void traverseHuffmanTree(HuffmanNode *node, char *code, int depth, char **codes)
{
    if (node->left == NULL && node->right == NULL)
    {
        code[depth] = '\0';
        codes[node->symbol] = strdup(code);
        return;
    }

    if (node->left != NULL)
    {
        code[depth] = '0';
        traverseHuffmanTree(node->left, code, depth + 1, codes);
    }

    if (node->right != NULL)
    {
        code[depth] = '1';
        traverseHuffmanTree(node->right, code, depth + 1, codes);
    }
}

// Função para comprimir os dados usando o algoritmo de Huffman
unsigned char *huffmanCompress(unsigned char *data, int size, int *compressedSize)
{
    HuffmanTable *table = buildHuffmanTable(data, size);
    HuffmanNode *tree = buildHuffmanTree(table);

    char **codes = (char **)malloc(256 * sizeof(char *));
    char *code = (char *)malloc(256 * sizeof(char));
    traverseHuffmanTree(tree, code, 0, codes);

    unsigned char *compressedData = (unsigned char *)malloc(size * sizeof(unsigned char));
    int index = 0;

    for (int i = 0; i < size; i++)
    {
        char *bitcode = codes[data[i]];
        int len = strlen(bitcode);
        for (int j = 0; j < len; j++)
        {
            compressedData[index / 8] <<= 1;
            compressedData[index / 8] |= (bitcode[j] - '0');
            index++;
        }
    }

    if (index % 8 != 0)
    {
        compressedData[index / 8] <<= (8 - (index % 8));
    }

    *compressedSize = (index + 7) / 8;

    for (int i = 0; i < 256; i++)
    {
        free(codes[i]);
    }
    free(codes);
    free(code);
    freeHuffmanTree(tree);
    freeHuffmanTable(table);

    return compressedData;
}

// Função para carregar uma imagem no formato PPM
typedef struct
{
    int largura;
    int altura;
    unsigned char *dados;
} Imagem;

Imagem *loadPPM(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }

    char format[3];
    fscanf(file, "%2s\n", format);

    if (format[0] != 'P' || format[1] != '6')
    {
        fprintf(stderr, "Invalid PPM format: %s\n", format);
        fclose(file);
        return NULL;
    }

    Imagem *image = (Imagem *)malloc(sizeof(Imagem));

    int c;
    while ((c = fgetc(file)) == '#')
    {
        while (fgetc(file) != '\n')
            ;
    }
    ungetc(c, file);

    fscanf(file, "%d %d\n", &(image->largura), &(image->altura));

    int maxIntensity;
    fscanf(file, "%d\n", &maxIntensity);

    image->dados = (unsigned char *)malloc(image->largura * image->altura * 3);
    fread(image->dados, 3, image->largura * image->altura, file);

    fclose(file);

    return image;
}

int main()
{
    const char *fileNames[] = {
        "Image1.ppm",
        "louis.ppm",
        "magazines.ppm",
        "debbiewarhol.ppm",
        "maxresdefault.ppm",
        "EricWSchwartz.ppm"};

    int numFiles = sizeof(fileNames) / sizeof(fileNames[0]);

    for (int i = 0; i < numFiles; i++)
    {
        const char *fileName = fileNames[i];
        Imagem *image = loadPPM(fileName);
        if (image)
        {
            printf("\nImagem: %s\n", fileName);
            printf("Dimensoes: %dx%d\n", image->largura, image->altura);

            int compressedSize;
            unsigned char *compressedData = huffmanCompress(image->dados, image->largura * image->altura * 3, &compressedSize);

            char compressedFileName[100];
            snprintf(compressedFileName, sizeof(compressedFileName), "huff_%s", fileName);

            FILE *compressedFile = fopen(compressedFileName, "wb");

            fprintf(compressedFile, "P6\n");
            fprintf(compressedFile, "%d %d\n", image->largura, image->altura);
            fprintf(compressedFile, "255\n");

            fwrite(compressedData, sizeof(unsigned char), compressedSize, compressedFile);

            fclose(compressedFile);

            int originalSize = image->largura * image->altura * 3;
            double compressionRatio = (1.0 - compressedSize / (double)originalSize) * 100;

            printf("Tamanho original: %d bytes\n", originalSize);
            printf("Tamanho comprimido: %d bytes\n", compressedSize);
            printf("Taxa de compressão: %.2f%%\n", compressionRatio);
            printf("Arquivo comprimido salvo como: %s\n", compressedFileName);
            printf("----------------------------------------------------------------");

            free(image->dados);
            free(image);
            free(compressedData);
        }
    }

    return 0;
}
