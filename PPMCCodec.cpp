#include "PPMCCodec.h"
#include "RangeCoder.h"
#include <random>

// Construtor
PPMCCodec::PPMCCodec(int K) : Kmax(K), model(K) {}

// Implementação da compressão
vector<uint8_t> PPMCCodec::compress(const vector<uint8_t>& data) {
    RangeEncoder enc;
    string history;
    if (Kmax > 0) history.reserve(Kmax);
    size_t progressInterval = max((size_t)1000, data.size() / 100);

    for (size_t i = 0; i <= data.size(); i++) {
        if (i > 0 && i % progressInterval == 0) {
            double progress = (double)i / data.size() * 100.0;
            cout << "\rProgresso: " << fixed << setprecision(1) << progress << "%" << flush;
        }

        int sym = (i < data.size()) ? data[i] : EOF_SYM;
        vector<bool> excluded(ALPHABET_SIZE, false);

        for (int k = min((int)history.size(), Kmax); k >= 0; k--) {
            string ctx = (k == 0) ? "" : history.substr(history.size() - k);
            auto dist = model.buildDist(ctx, excluded, sym);

            if (dist.total == 0) continue;

            if (dist.hasSym) {
                OptimizedTrieNode* node = model.getRoot();
                if (!ctx.empty()) {
                    for (char c : ctx) node = node->getChild((unsigned char)c);
                }
                if (!node) continue;
                enc.encode(dist.cum[sym], dist.cum[sym] + node->stats.freq[sym], dist.total);
                goto symbol_encoded;
            } else {
                if (dist.total > dist.cum[ESC_INDEX]) {
                    enc.encode(dist.cum[ESC_INDEX], dist.total, dist.total);
                }
                OptimizedTrieNode* node = model.getRoot();
                if (!ctx.empty()) {
                    for (char c : ctx) {
                        node = node->getChild((unsigned char)c);
                        if (!node) break;
                    }
                    if (node) {
                        for (int s = 0; s < ALPHABET_SIZE; s++) {
                            if (node->stats.freq[s] > 0) excluded[s] = true;
                        }
                    }
                }
            }
        }

        {
            auto dist = model.buildOrderMinusOne(excluded);
            if (dist.total == 0) {
                cerr << "ERRO FATAL: Simbolo nao codificavel." << endl;
                exit(1);
            }
            uint32_t cumLo = 0;
            for (int s = 0; s < sym; ++s) if (!excluded[s]) cumLo++;
            enc.encode(cumLo, cumLo + 1, dist.total);
        }

    symbol_encoded:
        model.update(history, sym);
        if (sym != EOF_SYM) {
            history.push_back((char)sym);
            if ((int)history.size() > Kmax) {
                history.erase(0, 1);
            }
        }
    }
    cout << "\r" << flush;
    enc.finish();
    return enc.getBytes();
}

// Implementação da descompressão
vector<uint8_t> PPMCCodec::decompress(const vector<uint8_t>& bits) {
    RangeDecoder dec(bits);
    vector<uint8_t> out;
    string history;
    if (Kmax > 0) history.reserve(Kmax);
    size_t iterations = 0;
    const size_t progressInterval = 100000;

    while (true) {
        if (++iterations % progressInterval == 0) {
            cout << "\rDescomprimindo... " << out.size() << " bytes" << flush;
        }

        vector<bool> excluded(ALPHABET_SIZE, false);
        int sym = -1;
        for (int k = min((int)history.size(), Kmax); k >= 0; k--) {
            string ctx = (k == 0) ? "" : history.substr(history.size() - k);
            auto dist = model.buildDist(ctx, excluded, -1);
            if (dist.total == 0) continue;
            uint32_t v = dec.getValue(dist.total);

            if (v >= dist.cum[ESC_INDEX]) {
                dec.decodeUpdate(dist.cum[ESC_INDEX], dist.total, dist.total);
                OptimizedTrieNode* node = model.getRoot();
                if (!ctx.empty()) {
                    for (char c : ctx) {
                        node = node->getChild((unsigned char)c);
                        if (!node) break;
                    }
                    if (node) {
                        for (int s = 0; s < ALPHABET_SIZE; s++) {
                            if (node->stats.freq[s] > 0) excluded[s] = true;
                        }
                    }
                }
            } else {
                auto it = upper_bound(dist.cum.begin(), dist.cum.begin() + ESC_INDEX, v);
                sym = distance(dist.cum.begin(), it) - 1;
                OptimizedTrieNode* node = model.getRoot();
                if (!ctx.empty()) {
                    for (char c : ctx) node = node->getChild((unsigned char)c);
                }
                if (sym < 0 || !node || node->stats.freq.at(sym) == 0) {
                    throw runtime_error("Erro de decodificacao: simbolo invalido encontrado.");
                }
                dec.decodeUpdate(dist.cum[sym], dist.cum[sym] + node->stats.freq[sym], dist.total);
                goto symbol_decoded;
            }
        }

        {
            auto dist = model.buildOrderMinusOne(excluded);
            if (dist.total == 0) throw runtime_error("Erro fatal: modelo de ordem -1 vazio.");
            uint32_t v = dec.getValue(dist.total);
            int count = 0;
            for (int s = 0; s < ALPHABET_SIZE; ++s) {
                if (!excluded[s]) {
                    if (count == (int)v) {
                        sym = s;
                        break;
                    }
                    count++;
                }
            }
            uint32_t cumLo = 0;
            for (int s = 0; s < sym; ++s) if (!excluded[s]) cumLo++;
            dec.decodeUpdate(cumLo, cumLo + 1, dist.total);
        }

    symbol_decoded:
        if (sym == EOF_SYM) break;
        out.push_back((uint8_t)sym);
        model.update(history, sym);
        history.push_back((char)sym);
        if ((int)history.size() > Kmax) {
            history.erase(0, 1);
        }
    }
    cout << "\r" << flush;
    return out;
}

// Implementação da geração de texto
string PPMCCodec::generateText(const vector<uint8_t>& training_data, size_t length, const string& seed) {
    // --- Fase 1: Treinamento do Modelo ---
    cout << "Treinando o modelo com " << training_data.size() << " bytes..." << endl;
    string history;
    if (Kmax > 0) history.reserve(Kmax);

    size_t progressInterval = max((size_t)1000, training_data.size() / 100);
    for (size_t i = 0; i < training_data.size(); ++i) {
        if (i > 0 && i % progressInterval == 0) {
            double progress = (double)i / training_data.size() * 100.0;
            cout << "\rProgresso do Treinamento: " << fixed << setprecision(1) << progress << "%" << flush;
        }
        model.update(history, training_data[i]);
        history.push_back((char)training_data[i]);
        if ((int)history.size() > Kmax) {
            history.erase(0, 1);
        }
    }
    cout << "\rTreinamento concluído." << endl;

    // --- Fase 2: Geração de Texto ---
    cout << "Gerando " << length << " caracteres de texto..." << endl;
    string generated_text = seed;
    history = seed;
    if ((int)history.size() > Kmax) {
        history = history.substr(history.size() - Kmax);
    }

    std::mt19937 rng(std::random_device{}()); 

    for (size_t i = 0; i < length; ++i) {
        vector<bool> excluded(ALPHABET_SIZE, false);
        int sym = -1;

        for (int k = min((int)history.size(), Kmax); k >= 0; k--) {
            string ctx = (k == 0) ? "" : history.substr(history.size() - k);
            auto dist = model.buildDist(ctx, excluded, -1);
            if (dist.total == 0) continue;

            std::uniform_int_distribution<uint32_t> uni(0, dist.total - 1);
            uint32_t v = uni(rng);

            if (v >= dist.cum[ESC_INDEX]) {
                OptimizedTrieNode* node = model.getRoot();
                if(!ctx.empty()){
                    // CORREÇÃO: Adicionada verificação de segurança no loop
                    for(char c : ctx) {
                        node = node->getChild((unsigned char)c);
                        if (!node) break;
                    }
                }
                if (node) {
                    for (int s = 0; s < ALPHABET_SIZE; s++) {
                        if (node->stats.freq[s] > 0) excluded[s] = true;
                    }
                }
            } else {
                auto it = upper_bound(dist.cum.begin(), dist.cum.begin() + ESC_INDEX, v);
                sym = distance(dist.cum.begin(), it) - 1;
                goto symbol_found;
            }
        }

        {
            auto dist = model.buildOrderMinusOne(excluded);
            if (dist.total == 0) { sym = ' '; }
            else {
                std::uniform_int_distribution<uint32_t> uni(0, dist.total - 1);
                uint32_t v = uni(rng);
                int count = 0;
                for (int s = 0; s < ALPHABET_SIZE; ++s) {
                    if (!excluded[s]) {
                        if (count == (int)v) {
                            sym = s;
                            break;
                        }
                        count++;
                    }
                }
            }
        }

    symbol_found:
        if (sym == EOF_SYM) {
            cout << "\nSímbolo EOF gerado. Fim da geração." << endl;
            break; 
        }

        generated_text += (char)sym;
        history.push_back((char)sym);
        if ((int)history.size() > Kmax) {
            history.erase(0, 1);
        }
    }

    return generated_text;
}

