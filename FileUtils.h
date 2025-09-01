#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "common.h"

// Funções utilitárias para ler e escrever arquivos.
namespace FileUtils {
    vector<uint8_t> readFile(const string& path);
    void writeFile(const string& path, const vector<uint8_t>& data);
}

#endif // FILEUTILS_H