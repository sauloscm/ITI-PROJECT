#ifndef BITSTREAM_H
#define BITSTREAM_H

#include "common.h"

// Classe para escrever bits individuais em um buffer.
class BitWriter {
public:
    vector<uint8_t> out;
    void putBit(int b);
    void flushBits();

private:
    uint8_t buf = 0;
    int bits = 0;
};

// Classe para ler bits individuais de um buffer.
class BitReader {
public:
    explicit BitReader(const vector<uint8_t>& v);
    int getBit();

private:
    const vector<uint8_t>& in;
    size_t pos = 0;
    uint8_t buf = 0;
    int bits = 0;
};

#endif // BITSTREAM_H
