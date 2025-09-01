#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <stdexcept>
#include <numeric>
#include <memory>
#include <array>
#include <iomanip>
#include <filesystem>

// Usings comuns
using namespace std;
namespace fs = std::filesystem;

// Configurações de Memória e Modelo
static constexpr size_t MAX_NODES = 20000000;
static constexpr int MIN_CONTEXT_FREQ = 2;
static constexpr int MAX_SYMBOL_FREQ = 32767;
static constexpr int FREQ_SCALE_THRESHOLD = 16384;

// Constantes do Alfabeto
static constexpr int ALPHABET_SIZE = 257;
static constexpr int EOF_SYM = 256;
static constexpr int ESC_INDEX = ALPHABET_SIZE;

#endif // COMMON_H
