#!/bin/bash

# --- Configurações ---
# Use o caminho no formato Linux (com barra /)
# O caminho abaixo é o mesmo que você verificou com 'ls'
CORPUS_DIR="/c/Users/saulo/OneDrive/Desktop/AREA DE TRABALHO/facu/2025.1/ITI/ppm2/PROJETO_1/corpus_total"
RESULTS_FILE="results.csv"
EXECUTABLE="./ppmc.exe"

# --- Verificações Iniciais ---
echo "INICIANDO SCRIPT DE TESTES..."
echo "Verificando diretorio do corpus: $CORPUS_DIR"
if [ ! -d "$CORPUS_DIR" ]; then
    echo "ERRO: O diretorio do corpus nao foi encontrado! Verifique o caminho em CORPUS_DIR."
    exit 1
fi
if [ ! -f "$EXECUTABLE" ]; then
    echo "ERRO: Executavel do compressor '$EXECUTABLE' nao encontrado. Compile o programa C++ primeiro."
    exit 1
fi

echo "Arquivo,Kmax,Original,Comprimido,Taxa(%),BitsPorSimbolo,TempoCompressao(ms),TempoDescompressao(ms)" > "$RESULTS_FILE"

# --- Laço principal para processar todos os arquivos ---
shopt -s nullglob
for f in "$CORPUS_DIR"/*; do
  if [ -f "$f" ]; then
    FILENAME=$(basename "$f")
    echo "Processando ARQUIVO: $FILENAME"

    for k in {0..10}; do
      echo "  -> Kmax = $k"
      COMPRESSED_FILE="temp_k${k}.ppmc"
      DECOMPRESSED_FILE="temp_k${k}.decomp"

      # Comprimir e capturar stats
      COMP_STATS=$( "$EXECUTABLE" c $k "$f" "$COMPRESSED_FILE" 2>&1 )
      COMP_EXIT_CODE=$?
      echo "--- Saida da Compressao (Kmax=$k) ---"
      echo "$COMP_STATS"

      # Descomprimir e capturar stats
      DECOMP_STATS=$( "$EXECUTABLE" d $k "$COMPRESSED_FILE" "$DECOMPRESSED_FILE" 2>&1 )
      DECOMP_EXIT_CODE=$?
      echo "--- Saida da Descompressao (Kmax=$k) ---"
      echo "$DECOMP_STATS"

      # --- Análise e Verificação ---
      if [ $COMP_EXIT_CODE -ne 0 ] || [ $DECOMP_EXIT_CODE -ne 0 ]; then
        echo "  -> ERRO GRAVE: Falha no executavel. Pulando Kmax=$k para este arquivo."
        echo "   Mensagem de erro da compressao: $COMP_STATS"
        echo "   Mensagem de erro da descompressao: $DECOMP_STATS"
        continue
      fi

      # Extrai os dados do output do programa
      ORIG_SIZE=$(echo "$COMP_STATS" | grep "Tamanho original" | awk '{print $3}')
      COMP_SIZE=$(echo "$COMP_STATS" | grep "Tamanho comprimido" | awk '{print $3}')
      RATIO=$(echo "$COMP_STATS" | grep "Taxa de compressao" | awk '{print $4}')
      BPS=$(echo "$COMP_STATS" | grep "Bits por simbolo" | awk '{print $4}')
      COMP_TIME=$(echo "$COMP_STATS" | grep "Tempo de compressao" | awk '{print $4}')
      DECOMP_TIME=$(echo "$DECOMP_STATS" | grep "Tempo de descompressao" | awk '{print $4}')

      if [ ! -f "$DECOMPRESSED_FILE" ]; then
        echo "  -> ERRO DE VERIFICACAO: Arquivo descomprimido nao foi criado. Algo deu muito errado."
      elif diff -q "$f" "$DECOMPRESSED_FILE" > /dev/null; then
          echo "  -> Verificacao OK"
      else
          echo "  -> ERRO DE VERIFICACAO: Arquivo original e descomprimido sao diferentes!"
      fi

      echo "$FILENAME,$k,$ORIG_SIZE,$COMP_SIZE,$RATIO,$BPS,$COMP_TIME,$DECOMP_TIME" >> "$RESULTS_FILE"
      rm -f "$COMPRESSED_FILE" "$DECOMPRESSED_FILE"
    done
  else
    echo "--> Pulando '$f' porque nao e um arquivo regular."
  fi
done

echo "Testes concluidos. Resultados em $RESULTS_FILE"