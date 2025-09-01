#ifndef PPMCCODEC_H
#define PPMCCODEC_H

#include "PPMCModel.h"

// A classe que une o modelo e o RangeCoder para realizar a compressão/descompressão.
class PPMCCodec {
public:
    explicit PPMCCodec(int K);
    vector<uint8_t> compress(const vector<uint8_t>& data);
    vector<uint8_t> decompress(const vector<uint8_t>& bits);

    // NOVA FUNÇÃO para gerar texto
    string generateText(const vector<uint8_t>& training_data, size_t length, const string& seed);

    
    const PPMCModel& getModel() const { return model; }

private:
    int Kmax;
    PPMCModel model;
};

#endif // PPMCCODEC_H
