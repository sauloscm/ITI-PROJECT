#ifndef ARCHIVER_H
#define ARCHIVER_H

#include "common.h"

// Funções para lidar com o arquivamento de diretórios.
namespace Archiver {
    vector<uint8_t> serializeDirectory(const string& inputPath);
    void deserializeAndWriteDirectory(const vector<uint8_t>& data, const string& outputPath);
}

#endif // ARCHIVER_H