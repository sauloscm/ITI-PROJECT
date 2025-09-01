#include "BitStream.h"

// --- BitWriter ---
void BitWriter::putBit(int b) {
    buf = (buf << 1) | (b & 1);
    bits++;
    if (bits == 8) {
        out.push_back(buf);
        buf = 0;
        bits = 0;
    }
}

void BitWriter::flushBits() {
    if (bits > 0) {
        buf <<= (8 - bits);
        out.push_back(buf);
        buf = 0;
        bits = 0;
    }
}

// --- BitReader ---
BitReader::BitReader(const vector<uint8_t>& v) : in(v) {}

int BitReader::getBit() {
    if (bits == 0) {
        if (pos >= in.size()) return 0;
        buf = in[pos++];
        bits = 8;
    }
    int b = (buf >> 7) & 1;
    buf <<= 1;
    bits--;
    return b;
}
