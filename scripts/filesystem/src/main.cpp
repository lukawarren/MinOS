#include <experimental/filesystem>
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
    for (const auto& entry : std::experimental::filesystem::directory_iterator(rootPath))
        vSizes.push_back(std::experimental::filesystem::file_size(entry));

    // Iterate through each file
    int i = 1;
    size_t totalBytes = 0;
    for (const auto& entry : std::experimental::filesystem::directory_iterator(rootPath))
    {
        std::ifstream fileHandle(entry.path(), std::ios::binary);
        std::vector<unsigned char> fileData(std::istreambuf_iterator<char>(fileHandle), {});

        // File header
        File file = {};
        file.size = std::experimental::filesystem::file_size(entry);
        strncpy(file.sName, entry.path().filename().generic_string().c_str(), MAX_FILE_NAME_LENGTH);
        file.sName[MAX_FILE_NAME_LENGTH-1] = '\0';

        // Set file linked list attribute
        if (i == vSizes.size()) file.pNext == 0;
        else
        {
            totalBytes += file.size + sizeof(File);
            file.pNext = totalBytes;
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