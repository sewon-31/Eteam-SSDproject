#pragma once

#include <string>

using std::string;

class NandData
{
public:
    enum LBA {
        MIN = 0,
        MAX = 99
    };

    string read(int lba) const;
    void write(int lba, const string& value);
    void erase(int startLBA, int endLBA);
    void clear();

    // updateFromNandFile()
    // updateFromBuffer()
    // updateNandFile()

private:
    bool isInvalidLBA(int lba) const;
    string data[100];
};