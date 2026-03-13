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
