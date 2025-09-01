#ifndef RANGECODER_H
#define RANGECODER_H

#include "BitStream.h"

// Implementa o codificador aritmético de inteiros.
class RangeEncoder {
public:
    // Construtor
    RangeEncoder();

    void encode(uint32_t cumLo, uint32_t cumHi, uint32_t total);
    void finish();
    
    // Declaração do método para obter os bytes. A implementação está no .cpp
    const vector<uint8_t>& getBytes() const;

private:
    void outputBitPlusFollow(int bit);
    static constexpr uint32_t TOP = 0xFFFFFFFFu;
    uint32_t low;
    uint32_t high;
    uint32_t underflow;
    BitWriter bw;
};

// Implementa o decodificador aritmético de inteiros.
class RangeDecoder {
public:
    explicit RangeDecoder(const vector<uint8_t>& bytes);
    uint32_t getValue(uint32_t total);
    void decodeUpdate(uint32_t cumLo, uint32_t cumHi, uint32_t total);

private:
    static constexpr uint32_t TOP = 0xFFFFFFFFu;
    uint32_t low = 0, high = TOP, code = 0;
    BitReader br;
};

#endif // RANGECODER_H

