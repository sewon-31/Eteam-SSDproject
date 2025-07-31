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

    NandData(const string& filePath = "");

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
    bool isInvalidLBA(int lba) const;

    string data[LBA::SIZE];
    FileInterface file;
};