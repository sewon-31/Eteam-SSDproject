#pragma once
#include "file_interface.h"

#include <string>

using std::string;

class NandData
{
public:
    enum LBA {
        MIN = 0,
        MAX = 99,
        SIZE = 100
    };

    static NandData& getInstance(const std::string& path = "../ssd_nand.txt");

    string read(int lba) const;
    void write(int lba, const string& value);
    void erase(int startLBA, int endLBA);
    void clear();

    bool updateFromFile();
    //bool updateFromBuffer()
    bool updateToFile();

    // for unit test
    FileInterface& getNandFile();

private:
    NandData(const string& filePath);
    NandData(const NandData&) = delete;
    NandData& operator=(const NandData&) = delete;

    bool isInvalidLBA(int lba) const;

    string data[LBA::SIZE];
    FileInterface file;
};