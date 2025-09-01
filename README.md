# Compressor PPM-C com Otimização de Memória e Arquivamento

Este repositório contém uma implementação em C++ de um compressor-descompressor de dados de alta performance baseado no algoritmo **PPM-C (Prediction by Partial Matching)**. O projeto foi otimizado para lidar com arquivos e diretórios grandes sem consumir memória excessiva, utilizando uma estrutura de dados **Trie** e técnicas de controle de memória.

## Funcionalidades Principais

* **Compressão e Descompressão PPM-C**: Implementa o algoritmo PPM-C com um Kmax (ordem do contexto) configurável.
* **Modelo de Trie Otimizado**: Utiliza uma árvore de prefixos (Trie) para reduzir drasticamente o consumo de RAM e permitir Kmax elevados.
* **Gerenciamento de Memória**: Limite de nós na Trie e técnicas de escalonamento de frequência para arquivos muito grandes.
* **Suporte a Arquivos e Diretórios**: Comprime um arquivo individual ou arquiva e comprime um diretório inteiro.
* **Geração de Texto Probabilística**: Cria novo texto a partir de um corpus treinado, seguindo probabilidades estatísticas.

## Configuração do Ambiente

O repositório contém apenas o código-fonte. Para executar os testes, você precisa dos arquivos de dados:

1. **Baixe o Corpus Silesia**: [Silesia Corpus](http://sun.aei.polsl.pl/~sdeor/index.php?page=silesia)
2. **Crie a Pasta do Corpus**:

```bash
mkdir corpus_total
```

3. **Extraia os Arquivos**: Descomprima `silesia.zip` para dentro da pasta `corpus_total`.
4. **Gere o Corpus de Texto**: Execute o script Python para criar um corpus de 100MB a partir do Projeto Gutenberg.

```bash
pip install requests
python create_corpus.py
```

## Como Compilar

Necessário um compilador C++ com suporte a **C++17**:

```bash
g++ -std=c++17 -O3 -o ppmc main.cpp PPMCCodec.cpp PPMCModel.cpp RangeCoder.cpp BitStream.cpp Archiver.cpp FileUtils.cpp
```

* `-std=c++17`: Define o padrão do C++.
* `-O3`: Ativa otimização máxima.
* `-o ppmc`: Nome do executável de saída.

## Guia de Comandos

O programa aceita três modos principais: `c` (compressão), `d` (descompressão) e `g` (geração de texto).

### 1. Compressão (c)

**Comprimir um Arquivo Individual:**

```bash
./ppmc c <Kmax> <arquivo_de_entrada> <arquivo_de_saida.ppmc>
```

**Exemplo:**

```bash
./ppmc c 6 ./corpus_total/dickens compress/dickens.ppmc
```

**Comprimir um Diretório Inteiro:**

```bash
./ppmc c <Kmax> <diretorio_de_entrada> <arquivo_de_saida.ppmc>
```

**Exemplo:**

```bash
./ppmc c 8 ./corpus_total compress/corpus_total.ppmc
```

### 2. Descompressão (d)

**Descomprimir para um Arquivo:**

```bash
./ppmc d <Kmax> <arquivo_comprimido.ppmc> <arquivo_de_saida>
```

**Exemplo:**

```bash
./ppmc d 6 compress/dickens.ppmc descompress/dickens_descomprimido.txt
```

**Descomprimir para um Diretório:**

> A pasta de destino deve ser criada antes.

```bash
mkdir descompress/corpus_descomprimido
./ppmc d <Kmax> <arquivo_comprimido.ppmc> <diretorio_de_saida>
```

**Exemplo:**

```bash
./ppmc d 8 compress/corpus_total.ppmc descompress/corpus_descomprimido
```

### 3. Geração de Texto (g)

```bash
./ppmc g <Kmax> <caminho_corpus.txt> <tamanho_a_gerar> "<texto_semente>"
```

* `<Kmax>`: Kmax ótimo para o corpus (ex: 8 para `english_corpus.txt`).
* `<caminho_corpus.txt>`: Arquivo de texto para treinar o modelo.
* `<tamanho_a_gerar>`: Número de caracteres a serem gerados.
* `"<texto_semente>"`: Frase inicial para contexto.

**Exemplo:**

```bash
./ppmc g 8 ./english_corpus.txt 2000 "once upon a time"
```

### 4. Scripts de Teste

**Comparação Rápida com WinRAR:**

```bash
./compare_winrar.sh <caminho_para_entrada> <Kmax>
```

**Exemplo:**

```bash
./compare_winrar.sh ./corpus_total/mozilla 8
```

## Estrutura do Projeto

* `main.cpp`: Entrada principal, processa argumentos.
* `PPMCCodec (.h/.cpp)`: Coordena compressão, descompressão e geração de texto.
* `PPMCModel (.h/.cpp)`: Lógica do modelo PPM-C com Trie.
* `RangeCoder (.h/.cpp)`: Codificador/decodificador aritmético.
* `BitStream (.h/.cpp)`: Leitura e escrita de bits.
* `Archiver (.h/.cpp)`: Serialização e deserialização de diretórios.
* `FileUtils (.h/.cpp)`: Funções utilitárias de arquivos.
* `common.h`: Constantes e includes globais.
