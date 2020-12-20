#include <filesystem>
#include <iterator>
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "file.h"

const std::string rootPath = "./root";
const std::string output = "./output/filesystem.bin";

int main()
{   
    Filesystem fs;
    std::vector<uint8_t> bytes;

    // Get file sizes
    std::vector<size_t> vSizes;
    for (const auto& entry : std::filesystem::directory_iterator(rootPath))
        vSizes.push_back(entry.file_size());

    // Iterate through each file
    int i = 1;
    size_t totalBytes = 0;
    for (const auto& entry : std::filesystem::directory_iterator(rootPath))
    {
        std::ifstream fileHandle(entry.path(), std::ios::binary);
        std::vector<unsigned char> fileData(std::istreambuf_iterator<char>(fileHandle), {});

        // File header
        File file = {};
        file.size = entry.file_size();
        strncpy(file.sName, entry.path().filename().generic_string().c_str(), 32);
        file.sName[31] = '\0';

        // Set file linked list attribute
        if (i == vSizes.size()) file.pNext == nullptr;
        else
        {
            totalBytes += file.size + sizeof(File);
            file.pNext = (File*)totalBytes;
        }
        ++i;

        // Convert file header to bytes
        auto ptr = reinterpret_cast<uint8_t*>(&file);
        auto buffer = std::vector<uint8_t>(ptr, ptr + sizeof(File));
        bytes.insert(bytes.end(), buffer.begin(), buffer.end());

        // Append data to bytes
        bytes.insert(bytes.end(), fileData.begin(), fileData.end());
    }

    // Write bytes to file
    std::ofstream fOut(output, std::ios::out | std::ios::binary);
    fOut.write((char*)&bytes[0], bytes.size() * sizeof(uint8_t));
    fOut.close();

    std::cout << "Wrote to file " << output << std::endl;
}