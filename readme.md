# PPM Compressor (RLE, HUFFMAN, LZ78) 
Este código foi desenvolvido como parte da Avaliação 2 da disciplina de Organização e Arquivo de Dados do 3º semestre de Ciência da Computação - 23.1. O autor deste trabalho é Manoela Resende, com o perfil [manoelargc](https://github.com/manoelargc).

## Descrição do Código
O código consiste na implementação de três algoritmos de compressão: RLE (Run-Length Encoding), Huffman e LZ78 (Lempel-Ziv 78). O objetivo é comparar as taxas de compressão desses algoritmos para diferentes arquivos de imagem no formato PPM.

### RLE (Run-Length Encoding)
O algoritmo RLE é implementado pela função rle_compressor(). Ele comprime os dados da imagem identificando sequências repetidas de cores e substituindo-as por um contador e uma única ocorrência da cor. O algoritmo percorre os dados de entrada e verifica se a próxima cor é igual à cor atual. Se forem iguais, incrementa um contador; caso contrário, adiciona o contador e a cor atual aos dados comprimidos. O processo é repetido até percorrer todos os dados de entrada.

### Huffman
O algoritmo de Huffman é implementado pelas funções create_huffman_node(), destroy_huffman_tree(), build_frequency_table(), build_huffman_tree() e build_huffman_codes(). Ele realiza a compressão através da construção de uma árvore de Huffman, onde os símbolos mais frequentes são representados por códigos de tamanho menor. A função build_frequency_table() constrói uma tabela de frequências dos símbolos nos dados de entrada. A função build_huffman_tree() constrói a árvore de Huffman a partir da tabela de frequências. A função build_huffman_codes() constrói os códigos de Huffman a partir da árvore de Huffman. A função huffman_compressor() realiza a compressão propriamente dita, substituindo cada símbolo pelo seu código de Huffman correspondente.

### LZ78 (Lempel-Ziv 78)
O algoritmo LZ78 é implementado pela função lz78_compressor(). Ele comprime os dados identificando sequências de cores que já foram encontradas anteriormente e substituindo-as por uma referência ao índice correspondente no dicionário. O algoritmo percorre os dados de entrada e verifica se a próxima cor já existe no dicionário. Se existir, atualiza a cor atual e continua para a próxima cor. Caso contrário, adiciona o índice e a cor atual aos dados comprimidos e adiciona a próxima cor ao dicionário.

## Processamento de Imagens
A função load_ppm() é utilizada para carregar uma imagem PPM a partir de um arquivo. A função save_compressed_data() é utilizada para salvar os dados comprimidos em um arquivo. A função process_images() percorre uma lista de arquivos de imagem, realiza a compressão para cada um dos algoritmos e registra as taxas de compressão em um arquivo de texto.

## Execução
Para executar o código, siga as etapas abaixo:

- Certifique-se de ter o compilador C instalado em sua máquina.
- Faça o download do arquivo do projeto.
- Abra um terminal ou prompt de comando e navegue até o diretório onde o código está localizado.
- Compile o código com o seguinte comando: gcc -o compressors compressors.c.
- Execute o programa com o seguinte comando: ./compressors.
- O programa processará as imagens listadas no vetor filenames[] e salvará as informações de compressão no arquivo comparacoes_compressores.txt. 
- Os dados comprimidos para cada algoritmo serão salvos em arquivos separados na pasta "compressoes" com o formato especificado, seguindo o modelo "[compressor_type]-[img_name].ppm"

--------

Observação: É imprescíncivel para a execução do arquivo certificar que realizou download do arquivo [IMAGENS.zip](https://github.com/manoelargc/OAD/blob/main/IMAGENS.zip) e que esteja no mesmo diretório do código.