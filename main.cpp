#include "common.h"
#include "PPMCCodec.h"
#include "Archiver.h"
#include "FileUtils.h"

void printUsage(const char* progName) {
    cerr << "Uso:" << endl;
    cerr << "  Compressao: " << progName << " c <Kmax> <arquivo_ou_diretorio_entrada> <arquivo_saida>" << endl;
    cerr << "  Descompressao: " << progName << " d <Kmax> <arquivo_entrada> <arquivo_ou_diretorio_saida>" << endl;
    cerr << "  Geracao de Texto: " << progName << " g <Kmax> <corpus_treinamento> <tamanho_gerar> \"<texto_semente>\"" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    try {
        string mode = argv[1];
        
        if (mode == "c" || mode == "d") {
            if (argc != 5) {
                printUsage(argv[0]);
                return 1;
            }
            int Kmax = stoi(argv[2]);
            string inputPath = argv[3];
            string outputPath = argv[4];
            auto start = chrono::high_resolution_clock::now();
            PPMCCodec codec(Kmax);

            if (mode == "c") {
                cout << "Modo: Compressao, Kmax: " << Kmax << endl;
                cout << "Input: " << inputPath << ", Output: " << outputPath << endl;
                cout << "Limite de nos: " << MAX_NODES << ", Freq. minima contexto: " << MIN_CONTEXT_FREQ << endl;

                vector<uint8_t> data;
                if (fs::is_directory(inputPath)) {
                    cout << "Tipo de entrada: Diretorio. Arquivando e comprimindo..." << endl;
                    data = Archiver::serializeDirectory(inputPath);
                } else if (fs::is_regular_file(inputPath)) {
                    cout << "Tipo de entrada: Arquivo. Comprimindo..." << endl;
                    data = FileUtils::readFile(inputPath);
                } else {
                    throw runtime_error("Caminho de entrada invalido ou nao existe.");
                }

                if (data.empty()) {
                    cout << "Entrada vazia. Nada a fazer." << endl;
                    FileUtils::writeFile(outputPath, {});
                    return 0;
                }

                auto comp = codec.compress(data);
                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

                double ratio = data.empty() ? 0 : (double)comp.size() / data.size();
                double bits_per_byte = data.empty() ? 0 : (double)(comp.size() * 8) / data.size();
                size_t nodeCount, memoryUsage;
                codec.getModel().getMemoryStats(nodeCount, memoryUsage);

                cout << "------------------------------------------" << endl;
                cout << "Tamanho original:     " << data.size() << " bytes" << endl;
                cout << "Tamanho comprimido:   " << comp.size() << " bytes" << endl;
                cout << "Taxa de compressao:   " << fixed << setprecision(4) << ratio * 100 << "%" << endl;
                cout << "Bits por simbolo:     " << fixed << setprecision(4) << bits_per_byte << endl;
                cout << "Tempo de compressao:  " << duration.count() << " ms" << endl;
                cout << "Nos da Trie:          " << nodeCount << endl;
                cout << "Memoria da Trie:      " << memoryUsage / 1024.0 / 1024.0 << " MB" << endl;
                cout << "Limite atingido:      " << (codec.getModel().isMemoryLimitReached() ? "SIM" : "NAO") << endl;
                cout << "------------------------------------------" << endl;

                FileUtils::writeFile(outputPath, comp);

            } else { // mode == "d"
                cout << "Modo: Descompressao, Kmax: " << Kmax << endl;
                cout << "Input: " << inputPath << ", Output: " << outputPath << endl;
                
                vector<uint8_t> comp = FileUtils::readFile(inputPath);
                if (comp.empty()) {
                    cout << "Arquivo de entrada vazio." << endl;
                    return 0;
                }
                
                auto decomp = codec.decompress(comp);
                
                if (fs::exists(outputPath) && fs::is_directory(outputPath)) {
                    cout << "Tipo de saida: Diretorio. Descomprimindo e desarquivando..." << endl;
                    Archiver::deserializeAndWriteDirectory(decomp, outputPath);
                } else {
                    cout << "Tipo de saida: Arquivo. Descomprimindo..." << endl;
                    FileUtils::writeFile(outputPath, decomp);
                }
                
                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
                cout << "------------------------------------------" << endl;
                cout << "Tamanho descomprimido:  " << decomp.size() << " bytes" << endl;
                cout << "Tempo de descompressao: " << duration.count() << " ms" << endl;
                cout << "------------------------------------------" << endl;
            }
        } else if (mode == "g") {
            if (argc != 6) {
                printUsage(argv[0]);
                return 1;
            }
            int Kmax = stoi(argv[2]);
            string corpusPath = argv[3];
            size_t length = stoul(argv[4]);
            string seed = argv[5];

            cout << "Modo: Geracao de Texto, Kmax: " << Kmax << endl;
            cout << "Corpus: " << corpusPath << ", Tamanho: " << length << ", Semente: \"" << seed << "\"" << endl;

            PPMCCodec codec(Kmax);
            vector<uint8_t> training_data = FileUtils::readFile(corpusPath);
            string generated_text = codec.generateText(training_data, length, seed);

            cout << "\n--- Texto Gerado ---\n" << endl;
            cout << generated_text << endl;
            cout << "\n--------------------" << endl;

        } else {
            cerr << "Modo desconhecido: " << mode << endl;
            printUsage(argv[0]);
            return 1;
        }
    } catch (const exception& e) {
        cerr << "Erro durante a execucao: " << e.what() << endl;
        return 1;
    }
    return 0;
}

