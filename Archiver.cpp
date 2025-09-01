#include "Archiver.h"
#include "FileUtils.h"

vector<uint8_t> Archiver::serializeDirectory(const string& inputPath) {
    vector<uint8_t> archive_data;
    size_t fileCount = 0;
    string basePath = inputPath;

    archive_data.resize(sizeof(size_t));

    for (const auto& entry : fs::recursive_directory_iterator(basePath)) {
        if (entry.is_regular_file()) {
            fileCount++;
            string relativePath = fs::relative(entry.path(), basePath).string();
            replace(relativePath.begin(), relativePath.end(), '\\', '/');
            size_t fileSize = fs::file_size(entry.path());

            size_t pathSize = relativePath.size();
            archive_data.insert(archive_data.end(), (uint8_t*)&pathSize, (uint8_t*)&pathSize + sizeof(size_t));
            archive_data.insert(archive_data.end(), (uint8_t*)relativePath.data(), (uint8_t*)relativePath.data() + pathSize);
            archive_data.insert(archive_data.end(), (uint8_t*)&fileSize, (uint8_t*)&fileSize + sizeof(size_t));

            vector<uint8_t> fileContent = FileUtils::readFile(entry.path().string());
            archive_data.insert(archive_data.end(), fileContent.begin(), fileContent.end());
        }
    }
    
    memcpy(archive_data.data(), &fileCount, sizeof(size_t));
    return archive_data;
}

void Archiver::deserializeAndWriteDirectory(const vector<uint8_t>& data, const string& outputPath) {
    size_t pos = 0;
    size_t fileCount;
    memcpy(&fileCount, data.data(), sizeof(size_t));
    pos += sizeof(size_t);

    for(size_t i = 0; i < fileCount; ++i) {
        size_t pathSize;
        memcpy(&pathSize, data.data() + pos, sizeof(size_t));
        pos += sizeof(size_t);

        string relativePath;
        relativePath.resize(pathSize);
        memcpy(relativePath.data(), data.data() + pos, pathSize);
        pos += pathSize;

        size_t fileSize;
        memcpy(&fileSize, data.data() + pos, sizeof(size_t));
        pos += sizeof(size_t);

        string fullPath = outputPath + "/" + relativePath;
        fs::path p(fullPath);
        fs::create_directories(p.parent_path());

        vector<uint8_t> fileContent(data.begin() + pos, data.begin() + pos + fileSize);
        FileUtils::writeFile(fullPath, fileContent);
        pos += fileSize;
    }
}
