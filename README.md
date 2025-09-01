Compressor PPM-C com Otimização de Memória e Arquivamento
Este repositório contém uma implementação em C++ de um compressor-descompressor de dados de alta performance baseado no algoritmo PPM-C (Prediction by Partial Matching). O projeto foi otimizado para lidar com arquivos e diretórios grandes sem consumir memória excessiva, utilizando uma estrutura de dados Trie e técnicas de controle de memória.

Funcionalidades Principais
Compressão e Descompressão PPM-C: Implementa o algoritmo PPM-C com um Kmax (ordem do contexto) configurável.

Modelo de Trie Otimizado: Utiliza uma árvore de prefixos (Trie) em vez de uma tabela hash, reduzindo drasticamente o consumo de RAM e permitindo o uso de Kmax elevados.

Gerenciamento de Memória: Inclui um limite de nós na Trie e técnicas de escalonamento de frequência para garantir que o programa não trave por falta de memória em arquivos muito grandes.

Suporte a Arquivos e Diretórios: Pode comprimir um único arquivo ou arquivar e comprimir um diretório inteiro em um único comando.

Geração de Texto Probabilística: Utiliza um modelo treinado a partir de um corpus de texto para gerar um novo texto, seguindo as probabilidades estatísticas da língua.

Como Compilar
Para compilar o projeto, é necessário um compilador C++ que suporte o padrão C++17 (devido ao uso da biblioteca filesystem).

Execute o seguinte comando no terminal (Git Bash, Linux, etc.):

g++ -std=c++17 -O3 -o ppmc main.cpp PPMCCodec.cpp PPMCModel.cpp RangeCoder.cpp BitStream.cpp Archiver.cpp FileUtils.cpp

-std=c++17: Define o padrão do C++.

-O3: Ativa o nível máximo de otimização de performance.

-o ppmc: Define o nome do arquivo executável de saída.

Guia de Comandos
O programa aceita três modos principais: c (compressão), d (descompressão) e g (geração de texto).

1. Compressão (c)
Comprimir um Arquivo Individual
./ppmc c <Kmax> <arquivo_de_entrada> <arquivo_de_saida.ppmc>

Exemplo:

./ppmc c 6 ./corpus_total/dickens compress/dickens.ppmc

Comprimir um Diretório Inteiro
./ppmc c <Kmax> <diretorio_de_entrada> <arquivo_de_saida.ppmc>

Exemplo:

./ppmc c 8 ./corpus_total compress/corpus_total.ppmc

2. Descompressão (d)
Descomprimir para um Arquivo
./ppmc d <Kmax> <arquivo_comprimido.ppmc> <arquivo_de_saida>

Exemplo:

./ppmc d 6 compress/dickens.ppmc descompress/dickens_descomprimido.txt

Descomprimir para um Diretório
Importante: A pasta de destino deve ser criada antes da execução.

# Passo 1: Criar a pasta
mkdir descompress/corpus_descomprimido

# Passo 2: Descomprimir
./ppmc d <Kmax> <arquivo_comprimido.ppmc> <diretorio_de_saida>

Exemplo:

./ppmc d 8 compress/corpus_total.ppmc descompress/corpus_descomprimido

3. Geração de Texto (g)
Este modo utiliza um corpus de texto para treinar o modelo PPM-C e, em seguida, gera um novo texto com base nas probabilidades aprendidas.

./ppmc g <Kmax> <caminho_corpus.txt> <tamanho_a_gerar> "<texto_semente>"

<Kmax>: O Kmax ótimo para o corpus (ex: 8 para o english_corpus.txt).

<caminho_corpus.txt>: O arquivo de texto para treinar o modelo.

<tamanho_a_gerar>: O número de caracteres a serem gerados.

"<texto_semente>": Uma frase inicial para dar contexto ao gerador. Deve estar entre aspas.

Exemplo:

./ppmc g 8 ./english_corpus.txt 2000 "once upon a time"

4. Scripts de Teste
Comparação Rápida com WinRAR
Este script compara o desempenho do ppmc com o WinRAR para uma entrada e Kmax específicos.

./compare_winrar.sh <caminho_para_entrada> <Kmax>

Exemplo:

./compare_winrar.sh ./corpus_total/mozilla 8

Estrutura do Projeto
O código foi refatorado em uma arquitetura orientada a objetos para facilitar a legibilidade e manutenção:

main.cpp: Ponto de entrada, processa os argumentos de linha de comando.

PPMCCodec (.h/.cpp): Orquestra o processo de compressão, descompressão e geração de texto.

PPMCModel (.h/.cpp): Implementa a lógica do modelo PPM-C com a Trie.

RangeCoder (.h/.cpp): Implementa o codificador e decodificador aritmético.

BitStream (.h/.cpp): Lida com a leitura e escrita de bits.

Archiver (.h/.cpp): Contém a lógica para serializar e deserializar diretórios.

FileUtils (.h/.cpp): Funções utilitárias para manipulação de arquivos.

common.h: Contém constantes e includes globais.
