#include "RangeCoder.h"

// --- RangeEncoder ---

// Implementação do construtor
RangeEncoder::RangeEncoder() : low(0), high(TOP), underflow(0) {}

void RangeEncoder::outputBitPlusFollow(int bit) {
    bw.putBit(bit);
    while (underflow > 0) {
        bw.putBit(!bit);
        underflow--;
    }
}

void RangeEncoder::encode(uint32_t cumLo, uint32_t cumHi, uint32_t total) {
    uint64_t range = (uint64_t)high - low + 1;
    high = low + (range * cumHi) / total - 1;
    low = low + (range * cumLo) / total;
    while (true) {
        if ((high & 0x80000000u) == (low & 0x80000000u)) {
            outputBitPlusFollow(high >> 31);
            low <<= 1;
            high = (high << 1) | 1;
        } else if ((low & 0x40000000u) && !(high & 0x40000000u)) {
            underflow++;
            low &= 0x3FFFFFFFu;
            high |= 0x40000000u;
            low <<= 1;
            high = (high << 1) | 1;
        } else {
            break;
        }
    }
}

void RangeEncoder::finish() {
    outputBitPlusFollow(1);
    bw.flushBits();
}

// Implementação do método getBytes
const vector<uint8_t>& RangeEncoder::getBytes() const {
    return bw.out;
}


// --- RangeDecoder ---
RangeDecoder::RangeDecoder(const vector<uint8_t>& bytes) : br(bytes) {
    for (int i = 0; i < 32; i++) {
        code = (code << 1) | br.getBit();
    }
}

uint32_t RangeDecoder::getValue(uint32_t total) {
    if (total == 0) return 0;
    uint64_t range = (uint64_t)high - low + 1;
    uint64_t val = ((uint64_t)(code - low) + 1) * total - 1;
    return (uint32_t)(val / range);
}

void RangeDecoder::decodeUpdate(uint32_t cumLo, uint32_t cumHi, uint32_t total) {
    uint64_t range = (uint64_t)high - low + 1;
    high = low + (range * cumHi) / total - 1;
    low = low + (range * cumLo) / total;
    while (true) {
        if ((high & 0x80000000u) == (low & 0x80000000u)) {
        } else if ((low & 0x40000000u) && !(high & 0x40000000u)) {
            code ^= 0x40000000u;
            low &= 0x3FFFFFFFu;
            high |= 0x40000000u;
        } else {
            break;
        }
        low <<= 1;
        high = (high << 1) | 1;
        code = (code << 1) | br.getBit();
    }
}

