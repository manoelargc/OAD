#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct
{
    int largura;
    int altura;
    unsigned char *dados;
} Imagem;

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

Imagem *carregar_ppm(const char *nome_arquivo)
{
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo)
    {
        fprintf(stderr, "Falha ao abrir o arquivo: %s\n", nome_arquivo);
        return NULL;
    }

    char formato[3];
    fscanf(arquivo, "%2s\n", formato); // Lê o formato do arquivo PPM (2 caracteres)

    if (formato[0] != 'P' || formato[1] != '6')
    {                                                           // Verifica se o formato é válido (P6)
        fprintf(stderr, "Formato PPM inválido: %s\n", formato); // Exibe uma mensagem de erro se o formato for inválido
        fclose(arquivo);                                        // Fecha o arquivo
        return NULL;
    }

    Imagem *imagem = malloc(sizeof(Imagem)); // Aloca memória para a estrutura de imagem

    // Ignora comentários
    int c;
    while ((c = fgetc(arquivo)) == '#')
    {
        while (fgetc(arquivo) != '\n')
            ;
    }
    ungetc(c, arquivo);

    fscanf(arquivo, "%d %d\n", &imagem->largura, &imagem->altura); // Lê a largura e altura da imagem

    int valor_maximo;
    fscanf(arquivo, "%d\n", &valor_maximo); // Lê o valor máximo para a intensidade de cor

    imagem->dados = malloc(imagem->largura * imagem->altura * 3);       // Aloca memória para os dados da imagem (3 bytes por pixel)
    fread(imagem->dados, 3, imagem->largura * imagem->altura, arquivo); // Lê os dados da imagem a partir do arquivo

    fclose(arquivo); // Fecha o arquivo
    return imagem;   // Retorna o ponteiro para a estrutura de imagem
}

/* void salvar_ppm(Imagem *imagem, unsigned char *dado_comprimido, int tam_comprimido, const char *nome_arquivo)
{
    FILE *arquivo = fopen(nome_arquivo, "wb");
    if (!arquivo)
    {
        fprintf(stderr, "Falha ao abrir o arquivo: %s\n", nome_arquivo);
        return;
    }

    fprintf(arquivo, "P6\n");
    fprintf(arquivo, "%d %d\n", imagem->largura, imagem->altura);
    fprintf(arquivo, "255\n");

    fwrite(dado_comprimido, sizeof(unsigned char), tam_comprimido, arquivo);

    fclose(arquivo);
} */

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

    for (int i = 0; i < numArquivos; i++)
    {
    const char *nomeArquivo = nomesArquivos[i];
    Imagem *imagem = carregar_ppm(nomeArquivo);
    if (imagem)
    {
        printf("\nImagem:%s\n", nomeArquivo);
        printf("Dimensoes: %dx%d\n", imagem->largura, imagem->altura);

        int tam_comprimido;
        unsigned char *dado_comprimido = rle_compressor(imagem->dados, imagem->largura * imagem->altura * 3, &tam_comprimido);



        // Criar o nome do arquivo de saída para a versão comprimida
        char nomeComprimido[100];
        snprintf(nomeComprimido, sizeof(nomeComprimido), "comprimido_%s", nomeArquivo);

        FILE *c = fopen(nomeComprimido, "wb");

        // Escrever o cabeçalho PPM no arquivo
        fprintf(c, "P6\n");
        fprintf(c, "%d %d\n", imagem->largura, imagem->altura);
        fprintf(c, "255\n");

        // Escrever os dados comprimidos no arquivo
        fwrite(dado_comprimido, sizeof(unsigned char), tam_comprimido, c);

        fclose(c);


        //free(dado);
        
        //salvar_ppm(imagem, dado_comprimido, tam_comprimido, "out.ppm");

        int tamanho_original = imagem->largura * imagem->altura * 3;
        double taxa_compressao = (1.0-tam_comprimido/(double)tamanho_original)*100;

        printf("Tamanho original: %d bytes\n", tamanho_original);
        printf("Tamanho comprimido: %d bytes\n", tam_comprimido);
        printf("Taxa de compresssao: %.2f%%\n", taxa_compressao);
        printf("Arquivo comprimido salvo como: %s\n", nomeComprimido);
        printf("----------------------------------------------------------------");
        free(imagem->dados);
        free(imagem);
        free(dado_comprimido);
    }
}
    return 0;
}
