#ifndef PPMCMODEL_H
#define PPMCMODEL_H

#include "common.h"

// Estrutura compacta para estatísticas de frequência.
struct CompactStats {
    array<uint16_t, ALPHABET_SIZE> freq;
    uint32_t totalFreq = 0;

    CompactStats();
    void add(int sym);
    void scale();
};

// Nó da árvore Trie otimizada.
struct OptimizedTrieNode {
    CompactStats stats;
    vector<pair<uint8_t, unique_ptr<OptimizedTrieNode>>> children;

    OptimizedTrieNode();
    OptimizedTrieNode* getChild(uint8_t symbol);
    OptimizedTrieNode* getOrCreateChild(uint8_t symbol);
};

// Classe principal do modelo PPMC, baseada na Trie.
class PPMCModel {
public:
    explicit PPMCModel(int K);
    void update(const string& history, int sym);

    struct Dist {
        vector<uint32_t> cum;
        uint32_t total = 0;
        bool hasSym = false;
    };

    Dist buildDist(const string& ctx, const vector<bool>& excluded, int targetSym);
    Dist buildOrderMinusOne(const vector<bool>& excluded);
    
    void getMemoryStats(size_t& nodes, size_t& memoryUsage) const;
    bool isMemoryLimitReached() const { return memoryLimitReached; }
    OptimizedTrieNode* getRoot() { return root.get(); }

private:
    int Kmax;
    unique_ptr<OptimizedTrieNode> root;
    size_t nodeCount = 0;
    size_t maxNodes;
    bool memoryLimitReached = false;
};

#endif // PPMCMODEL_H
