#include "PPMCModel.h"

// --- CompactStats ---
CompactStats::CompactStats() {
    freq.fill(0);
}

void CompactStats::add(int sym) {
    if (freq[sym] < MAX_SYMBOL_FREQ) {
        freq[sym]++;
        totalFreq++;
        if (totalFreq > FREQ_SCALE_THRESHOLD) {
            scale();
        }
    }
}

void CompactStats::scale() {
    uint32_t newTotal = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freq[i] = (freq[i] + 1) >> 1;
        newTotal += freq[i];
    }
    totalFreq = newTotal;
}

// --- OptimizedTrieNode ---
OptimizedTrieNode::OptimizedTrieNode() {
    children.reserve(4);
}

OptimizedTrieNode* OptimizedTrieNode::getChild(uint8_t symbol) {
    for (auto& child : children) {
        if (child.first == symbol) {
            return child.second.get();
        }
    }
    return nullptr;
}

OptimizedTrieNode* OptimizedTrieNode::getOrCreateChild(uint8_t symbol) {
    OptimizedTrieNode* existing = getChild(symbol);
    if (existing) return existing;
    children.emplace_back(symbol, make_unique<OptimizedTrieNode>());
    return children.back().second.get();
}

// --- PPMCModel ---
PPMCModel::PPMCModel(int K) : Kmax(K), maxNodes(MAX_NODES) {
    root = make_unique<OptimizedTrieNode>();
    nodeCount = 1;
}

void PPMCModel::update(const string& history, int sym) {
    for (int k = 0; k <= min((int)history.size(), Kmax); k++) {
        OptimizedTrieNode* node = root.get();
        if (k > 0) {
            string ctx = history.substr(history.size() - k);
            bool canCreateNodes = (nodeCount < maxNodes && !memoryLimitReached);
            for (char c : ctx) {
                uint8_t idx = (unsigned char)c;
                OptimizedTrieNode* child = node->getChild(idx);
                if (!child) {
                    if (!canCreateNodes || node->stats.totalFreq < MIN_CONTEXT_FREQ) {
                        goto next_order;
                    }
                    child = node->getOrCreateChild(idx);
                    nodeCount++;
                    if (nodeCount >= maxNodes) {
                        memoryLimitReached = true;
                        canCreateNodes = false;
                    }
                }
                node = child;
            }
        }
        node->stats.add(sym);
    next_order:;
    }
}

PPMCModel::Dist PPMCModel::buildDist(const string& ctx, const vector<bool>& excluded, int targetSym) {
    Dist d;
    d.cum.assign(ALPHABET_SIZE + 1, 0);
    OptimizedTrieNode* node = root.get();

    if (!ctx.empty()) {
        for (char c : ctx) {
            uint8_t idx = (unsigned char)c;
            node = node->getChild(idx);
            if (!node) {
                d.total = 1;
                return d;
            }
        }
    }

    const CompactStats& st = node->stats;
    uint32_t run = 0;
    int remainingDistinct = 0;
    for (int s = 0; s < ALPHABET_SIZE; s++) {
        d.cum[s] = run;
        if (st.freq[s] > 0 && !excluded[s]) {
            run += st.freq[s];
            remainingDistinct++;
            if (s == targetSym) d.hasSym = true;
        }
    }

    uint32_t escFreq = max(1, remainingDistinct);
    d.cum[ESC_INDEX] = run;
    d.total = run + escFreq;
    return d;
}

PPMCModel::Dist PPMCModel::buildOrderMinusOne(const vector<bool>& excluded) {
    Dist d;
    d.cum.assign(ALPHABET_SIZE, 0);
    uint32_t run = 0;
    for (int s = 0; s < ALPHABET_SIZE; s++) {
        d.cum[s] = run;
        if (!excluded[s]) {
            run++;
        }
    }
    d.total = run;
    return d;
}

void PPMCModel::getMemoryStats(size_t& nodes, size_t& memoryUsage) const {
    nodes = nodeCount;
    memoryUsage = nodeCount * (sizeof(OptimizedTrieNode));
    memoryUsage += nodeCount * 4 * (sizeof(uint8_t) + sizeof(unique_ptr<OptimizedTrieNode>));
}
