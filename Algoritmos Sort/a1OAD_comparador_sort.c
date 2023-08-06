#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//-------------------------------------------------------------------------------
/*AVALIAÇÃO COMPARADOR DE ALGORITIMOS SORT | OAD 2023.1 Manoela Resende 2210100235
//-------------------------------------------------------------------------------

comparar os seguintes algoritmos:
1. Bubble Sort, Insert Sort, Selection Sort.  ( O(n²) )
2. Merge Sort, Quick Sort, Heap Sort.  ( O (n x log(n))  )
3. implemente o Radix sort (O(w+n)) e inclua-o na sua comparação
Trace gráficos e analise o comportamento do grupo 1, do grupo 2 e compare os grupos */

int i, j, comparacoes; // Declaração da variável 'comparacoes' para contar o número de comparações

//-----------------------
// Protótipo das funções
//-----------------------

void vetor_aleatorio(int vet[], int tamanho);
void bubbleSort(int vet[], int tamanho);
void insertionSort(int vetor[], int tamanho);
void selectionSort(int vet[], int tamanho);
void mergeSort(int vet[], int tamanho);
void quickSort(int vet[], int menor_posicao, int maior_posicao);
void heapSort(int vet[], int tamanho);
void radixSort(int vet[], int tamanho);
void countingSort(int vet[], int tamanho, int exp);

//------------
// Execução
//------------

int main()
{

    //--------------------------------
    //========COMPARAÇÃO GERAL========
    //--------------------------------

    FILE *arquivo;
    arquivo = fopen("dados_comparacoes.dat", "w"); // Abre o arquivo em modo de escrita
    int tamanho;

    fprintf(arquivo, "#Tamanho, Bubble Sort, Insertion Sort, Selection Sort, Merge Sort, Quick Sort, Heap Sort, Radix Sort\n");
    // escreve o cabeçalho no arquivo com os nomes dos algoritmos de ordenação

    for (tamanho = 0; tamanho <= 100; tamanho += 5)
    {
        int vet[tamanho]; // cria um vetor com o tamanho atual

        vetor_aleatorio(vet, tamanho); // preenche o vetor com valores aleatórios

        comparacoes = 0; // Inicializa a variável 'comparacoes' como zero antes de cada execução

        bubbleSort(vet, tamanho);
        int bubbleSort_comparacoes = comparacoes;

        comparacoes = 0;
        insertionSort(vet, tamanho);
        int insertionSort_comparacoes = comparacoes;

        comparacoes = 0;
        selectionSort(vet, tamanho);
        int selectionSort_comparacoes = comparacoes;

        comparacoes = 0;
        mergeSort(vet, tamanho);
        int mergeSort_comparacoes = comparacoes;

        comparacoes = 0;
        quickSort(vet, 0, tamanho - 1);
        int quickSort_comparacoes = comparacoes;

        comparacoes = 0;
        heapSort(vet, tamanho);
        int heapSort_comparacoes = comparacoes;

        comparacoes = 0;
        radixSort(vet, tamanho);
        int radixSort_comparacoes = comparacoes;

        fprintf(arquivo, "%d, %d, %d, %d, %d, %d, %d, %d\n", tamanho, bubbleSort_comparacoes, insertionSort_comparacoes, selectionSort_comparacoes, mergeSort_comparacoes, quickSort_comparacoes, heapSort_comparacoes, radixSort_comparacoes);
    }

    fclose(arquivo);

    //--------------------------------
    //==========GRUPO 1===============
    //--------------------------------

    FILE *grupo1;
    grupo1 = fopen("dados_grupo1.dat", "w"); // Abre o arquivo em modo de escrita

    fprintf(grupo1, "#Tamanho, Bubble Sort, Insertion Sort, Selection Sort\n");

    for (tamanho = 0; tamanho <= 100; tamanho += 5)
    {
        int vet[tamanho];

        vetor_aleatorio(vet, tamanho);

        comparacoes = 0; // Inicializa a variável 'comparacoes' como zero antes de cada execução

        bubbleSort(vet, tamanho);
        int bubbleSort_comparacoes = comparacoes;

        comparacoes = 0;
        insertionSort(vet, tamanho);
        int insertionSort_comparacoes = comparacoes;

        comparacoes = 0;
        selectionSort(vet, tamanho);
        int selectionSort_comparacoes = comparacoes;

        fprintf(grupo1, "%d, %d, %d, %d\n", tamanho, bubbleSort_comparacoes, insertionSort_comparacoes, selectionSort_comparacoes);
    }

    fclose(grupo1);

    //--------------------------------
    //==========GRUPO 2===============
    //--------------------------------

    FILE *grupo2;
    grupo2 = fopen("dados_grupo2.dat", "w"); // Abre o arquivo em modo de escrita

    fprintf(grupo2, "#Tamanho, Merge Sort, Quick Sort, Heap Sort\n");

    for (tamanho = 0; tamanho <= 1000; tamanho += 10)
    {
        int vet[tamanho];

        vetor_aleatorio(vet, tamanho);

        comparacoes = 0; // Inicializa a variável 'comparacoes' como zero antes de cada execução
        mergeSort(vet, tamanho);
        int mergeSort_comparacoes = comparacoes;

        comparacoes = 0;
        quickSort(vet, 0, tamanho - 1);
        int quickSort_comparacoes = comparacoes;

        comparacoes = 0;
        heapSort(vet, tamanho);
        int heapSort_comparacoes = comparacoes;

        fprintf(arquivo, "%d, %d, %d, %d\n", tamanho, mergeSort_comparacoes, quickSort_comparacoes, heapSort_comparacoes);
    }
    fclose(grupo2);

    //--------------------------------
    //==========RADIX SORT============
    //--------------------------------

    FILE *arq_radix;
    arq_radix = fopen("dados_radix.dat", "w"); // Abre o arquivo em modo de escrita

    fprintf(arq_radix, "#Tamanho, Radix Sort\n");

    for (tamanho = 0; tamanho <= 10000; tamanho += 100)
    {
        int vet[tamanho];

        vetor_aleatorio(vet, tamanho);

        comparacoes = 0;
        radixSort(vet, tamanho);
        int radixSort_comparacoes = comparacoes;

        fprintf(arquivo, "%d, %d\n", tamanho, radixSort_comparacoes);
    }
    fclose(arq_radix);

    return 0;
}

//-------------------------------------------------------------------------
//============================== FUNÇÕES ==================================
//-------------------------------------------------------------------------

//----------------------
// gerar vetor aleatório
//----------------------
void vetor_aleatorio(int vet[], int tamanho)
{
    srand(time(NULL));
    for (i = 0; i < tamanho; i++)
    {
        vet[i] = rand() % 100; // Atribui valores aleatórios de 0 a 99 para cada elemento do vetor
    }
}

//----------------------
// GRUPO 1 (( O(n²) ))
//----------------------

// algoritmo bubble sort
void bubbleSort(int vet[], int tamanho)
{
    for (i = 0; i < tamanho - 1; i++) // percorre o vetor a partir do início até o penúlt.  imo elemento
    {
        for (j = 0; j < tamanho - i - 1; j++) // percorre até a posição corrente do elemento i
        {
            comparacoes++; //incrementa o numero de comparacoes
            if (vet[j] > vet[j + 1]) // compara os pares e realiza a troca se estiverem fora de ordem
            {
                int temp = vet[j]; // guarda temporariamente o valor do elemento atual
                vet[j] = vet[j + 1]; // faz a troca do elemento atual com o próximo elemento
                vet[j + 1] = temp; // guarda o valor temporário no próximo elemento
            }
        }
    }
}

// algoritmo insertion sort
void insertionSort(int vet[], int tamanho)
{
    for (i = 1; i < tamanho; i++)
    {
        int chave = vet[i];
        j = i - 1;

        comparacoes++; // Incrementa o número de comparações

        while (j >= 0 && vet[j] > chave)
        {
            vet[j + 1] = vet[j];
            j--;
            comparacoes++; // Incrementa o número de comparações
        }

        vet[j + 1] = chave;
    }
}

// algoritmo selection sort
void selectionSort(int vet[], int tamanho)
{
    for (i = 0; i < tamanho - 1; i++)
    {
        int menor_indice = i;

        for (j = i + 1; j < tamanho; j++)
        {
            comparacoes++; // Incrementa o número de comparações
            if (vet[j] < vet[menor_indice])
            {
                menor_indice = j;
            }
        }

        int temp = vet[i];
        vet[i] = vet[menor_indice];
        vet[menor_indice] = temp;
    }
}

//----------------------
// GRUPO 2 (O(n log n))
//----------------------

// algoritmo merge sort
void merge(int vet[], int esquerda, int meio, int direita)
{
    int i, j, k;
    int tamanho_esquerda = meio - esquerda + 1;
    int tamanho_direita = direita - meio;

    int vetor_esquerda[tamanho_esquerda], vetor_direita[tamanho_direita];

    for (i = 0; i < tamanho_esquerda; i++)
    {
        vetor_esquerda[i] = vet[esquerda + i];
    }

    for (j = 0; j < tamanho_direita; j++)
    {
        vetor_direita[j] = vet[meio + 1 + j];
    }

    i = 0;
    j = 0;
    k = esquerda;

    while (i < tamanho_esquerda && j < tamanho_direita)
    {
        comparacoes++; // Incrementa o número de comparações
        if (vetor_esquerda[i] <= vetor_direita[j])
        {
            vet[k] = vetor_esquerda[i];
            i++;
        }
        else
        {
            vet[k] = vetor_direita[j];
            j++;
        }
        k++;
    }

    while (i < tamanho_esquerda)
    {
        vet[k] = vetor_esquerda[i];
        i++;
        k++;
    }

    while (j < tamanho_direita)
    {
        vet[k] = vetor_direita[j];
        j++;
        k++;
    }
}

//--->funcao principal do merge sort
void mergeSort(int vet[], int tamanho)
{
    if (tamanho > 1)
    {
        int meio = tamanho / 2;

        int vetor_esquerda[meio];
        int vetor_direita[tamanho - meio];

        for (i = 0; i < meio; i++)
        {
            vetor_esquerda[i] = vet[i];
        }

        for (i = meio; i < tamanho; i++)
        {
            vetor_direita[i - meio] = vet[i];
        }

        mergeSort(vetor_esquerda, meio);
        mergeSort(vetor_direita, tamanho - meio);

        merge(vet, 0, meio - 1, tamanho - 1);
    }
}


// algoritmo quick sort

//--->função que realiza a partição do vetor para o quick sort
int particao(int vet[], int menor_posicao, int maior_posicao)
{
    int pivo = vet[maior_posicao];
    int i = (menor_posicao - 1);

    for (j = menor_posicao; j <= maior_posicao - 1; j++)
    {
        comparacoes++; // Incrementa o número de comparações
        if (vet[j] < pivo)
        {
            i++;
            int temp = vet[i];
            vet[i] = vet[j];
            vet[j] = temp;
        }
    }

    int temp = vet[i + 1];
    vet[i + 1] = vet[maior_posicao];
    vet[maior_posicao] = temp;

    // retorna a posição do pivo após a partição
    return (i + 1);
}

//--->funcao principal quick sort
void quickSort(int vet[], int menor_posicao, int maior_posicao)
{
    if (menor_posicao < maior_posicao)
    {
        int pivo = particao(vet, menor_posicao, maior_posicao);

        quickSort(vet, menor_posicao, pivo - 1);
        quickSort(vet, pivo + 1, maior_posicao);
    }
}

// algoritmo heap sort

//--->função responsável por criar uma estrutura de heap máxima no vetor.
void heapify(int vet[], int tamanho, int indice_raiz)
{
    int maior = indice_raiz;
    int filho_esquerda = 2 * indice_raiz + 1;
    int filho_direita = 2 * indice_raiz + 2;

    if (filho_esquerda < tamanho)
    {
        comparacoes++; // Incrementa o número de comparações
        if (vet[filho_esquerda] > vet[maior])
        {
            maior = filho_esquerda;
        }
    }

    if (filho_direita < tamanho)
    {
        comparacoes++; // Incrementa o número de comparações
        if (vet[filho_direita] > vet[maior])
        {
            maior = filho_direita;
        }
    }

    if (maior != indice_raiz)
    {
        int temp = vet[indice_raiz];
        vet[indice_raiz] = vet[maior];
        vet[maior] = temp;

        heapify(vet, tamanho, maior);
    }
}

//--->função principal do heap sort
void heapSort(int vet[], int tamanho)
{
    for (i = tamanho / 2 - 1; i >= 0; i--)
    {
        heapify(vet, tamanho, i);
    }

    for (i = tamanho - 1; i > 0; i--)
    {
        int temp = vet[0];
        vet[0] = vet[i];
        vet[i] = temp;

        heapify(vet, i, 0);
    }
}

//----------------------
// RADIX SORT (( O(w+n) ))
//----------------------

//--->função para obter o valor máximo no vetor
int valor_maximo(int vet[], int tamanho)
{
    int max = vet[0];
    for (i = 1; i < tamanho; i++)
    {
        comparacoes++; // Incrementa o número de comparações
        if (vet[i] > max)
        {
            max = vet[i];
        }
    }
    return max;
}


//--->função de ordenação counting sort para um determinado dígito (exp)
void countSort(int vet[], int tamanho, int exp)
{
    int vetor_saida[tamanho];
    int count[10] = {0};

    // contagem do número de ocorrências de cada dígito
    for (i = 0; i < tamanho; i++)
    {
        count[(vet[i] / exp) % 10]++;
    }

    // atualização do vetor count para indicar a posição correta de cada dígito na saída
    for (i = 1; i < 10; i++)
    {
        count[i] += count[i - 1];
    }

    // construção do vetor de saída ordenado
    for (i = tamanho - 1; i >= 0; i--)
    {
        vetor_saida[count[(vet[i] / exp) % 10] - 1] = vet[i];
        count[(vet[i] / exp) % 10]--;
    }

    // copiar o vetor de saída para o vetor original
    for (i = 0; i < tamanho; i++)
    {
        vet[i] = vetor_saida[i];
    }
}


//--->função principal para executar o radix sort
void radixSort(int vet[], int tamanho)
{
    int max = valor_maximo(vet, tamanho);
    int exp; //define o digito atual

    //executa o count sort p/ cada digito, do menos significativo ao mais significativo
    for (exp = 1; max / exp > 0; exp *= 10)
    {
        countSort(vet, tamanho, exp);
    }
}
