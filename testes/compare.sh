#!/bin/bash

# --- Script para comparar PPM-C com WinRAR (arquivos e diretórios) ---
# --- Ex (./compare.sh c ./corpus_total/dickens 8)
# --- Configurações (ajuste os caminhos se necessário) ---

PPMC_EXECUTABLE="./ppmc"
WINRAR_EXECUTABLE="C:\\Program Files\\WinRAR\\WinRAR.exe"
WINRAR_ALGORITHM="RAR5 (Best)"

# --- Verifica se os argumentos foram fornecidos ---
if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
    echo "Uso: ./compare.sh <c|d> <caminho_entrada> <Kmax>"
    echo "Exemplo de Compressao: ./compare.sh c ./corpus_total/dickens 6"
    echo "Exemplo de Descompressao: ./compare.sh d ./dickens.ppmc 6"
    exit 1
fi

MODE="$1"
INPUT_PATH="$2"
KMAX_PPM="$3"

echo "---------------------------------------------------------"
echo "Modo: $MODE, Kmax: $KMAX_PPM"
echo "Entrada: $INPUT_PATH"
echo "---------------------------------------------------------"

if [ "$MODE" == "c" ]; then
    # --- Lógica para Compressão ---
    if [ ! -f "$INPUT_PATH" ] && [ ! -d "$INPUT_PATH" ]; then
        echo "ERRO: Arquivo ou diretório de entrada não encontrado."
        exit 1
    fi
    
    FILENAME=$(basename "$INPUT_PATH")
    ORIG_SIZE=$(du -sb "$INPUT_PATH" | awk '{print $1}')
    
    echo "Arquivo Original: $FILENAME (Tamanho: $ORIG_SIZE bytes)"
    
    # Teste com PPM-C
    echo "Compressor: PPM-C"
    COMPRESSED_FILE="temp.ppmc"
    COMP_STATS=$( "$PPMC_EXECUTABLE" c $KMAX_PPM "$INPUT_PATH" "$COMPRESSED_FILE" 2>&1 )
    if [ -f "$COMPRESSED_FILE" ]; then
      COMP_SIZE=$(echo "$COMP_STATS" | grep "Tamanho comprimido" | awk '{print $3}')
      COMP_TIME=$(echo "$COMP_STATS" | grep "Tempo de compressao" | awk '{print $4}')
      RATIO=$(awk "BEGIN {printf \"%.2f\", ($COMP_SIZE / $ORIG_SIZE) * 100}")
      BPS=$(awk "BEGIN {printf \"%.2f\", ($COMP_SIZE * 8) / $ORIG_SIZE}")
      echo "  -> Taxa: $RATIO%"
      echo "  -> BPS: $BPS"
      echo "  -> Tempo: $COMP_TIME ms"
      rm -f "$COMPRESSED_FILE"
    else
      echo "  -> ERRO na compressao do PPM-C."
    fi

    # Teste com WinRAR
    echo "Compressor: WinRAR ($WINRAR_ALGORITHM)"
    COMPRESSED_FILE="temp.rar"
    START_TIME=$(date +%s%N)
    "$WINRAR_EXECUTABLE" a -ep1 -m5 "$COMPRESSED_FILE" "$INPUT_PATH" > /dev/null
    END_TIME=$(date +%s%N)
    COMP_TIME=$(( ($END_TIME - $START_TIME) / 1000000 ))
    if [ -f "$COMPRESSED_FILE" ]; then
      WINRAR_SIZE=$(stat --printf="%s" "$COMPRESSED_FILE")
      RATIO=$(awk "BEGIN {printf \"%.2f\", ($WINRAR_SIZE / $ORIG_SIZE) * 100}")
      BPS=$(awk "BEGIN {printf \"%.2f\", ($WINRAR_SIZE * 8) / $ORIG_SIZE}")
      echo "  -> Taxa: $RATIO%"
      echo "  -> BPS: $BPS"
      echo "  -> Tempo: $COMP_TIME ms"
      rm -f "$COMPRESSED_FILE"
    else
      echo "  -> ERRO na compressao do WinRAR."
    fi

elif [ "$MODE" == "d" ]; then
    # --- Lógica para Descompressão ---
    if [ ! -f "$INPUT_PATH" ]; then
        echo "ERRO: Arquivo comprimido de entrada não encontrado."
        exit 1
    fi
    
    # Descomprime com PPM-C
    echo "Descompressor: PPM-C"
    PPMC_DECOMP_OUTPUT="temp.decomp"
    START_TIME=$(date +%s%N)
    "$PPMC_EXECUTABLE" d $KMAX_PPM "$INPUT_PATH" "$PPMC_DECOMP_OUTPUT" > /dev/null 2>&1
    END_TIME=$(date +%s%N)
    COMP_TIME=$(( ($END_TIME - $START_TIME) / 1000000 ))
    echo "  -> Tempo: $COMP_TIME ms"
    rm -f "$PPMC_DECOMP_OUTPUT"

    # Descomprime com WinRAR
    echo "Descompressor: WinRAR"
    WINRAR_DECOMP_OUTPUT="temp.winrar_decomp"
    START_TIME=$(date +%s%N)
    "$WINRAR_EXECUTABLE" x "$INPUT_PATH" "$WINRAR_DECOMP_OUTPUT" > /dev/null
    END_TIME=$(date +%s%N)
    COMP_TIME=$(( ($END_TIME - $START_TIME) / 1000000 ))
    echo "  -> Tempo: $COMP_TIME ms"
    rm -rf "$WINRAR_DECOMP_OUTPUT"
    
else
    echo "ERRO: Modo inválido. Use 'c' para compressão ou 'd' para descompressão."
    exit 1
fi