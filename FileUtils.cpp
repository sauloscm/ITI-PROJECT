#include "FileUtils.h"

vector<uint8_t> FileUtils::readFile(const string& path) {
    ifstream in(path, ios::binary);
    if (!in) {
        throw runtime_error("Erro abrindo arquivo: " + path);
    }
    return vector<uint8_t>((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
}

void FileUtils::writeFile(const string& path, const vector<uint8_t>& data) {
    ofstream out(path, ios::binary);
    if (!out) {
        throw runtime_error("Erro criando arquivo: " + path);
    }
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
}
