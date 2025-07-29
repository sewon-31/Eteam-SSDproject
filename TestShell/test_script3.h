#pragma once
#include <string>
#include "test_shell.h"

class TestScript3 {
public:
    void writeReadAging(SSDInterface& ssd);
    static bool ReadCompare(SSDInterface& ssd, int lba, const std::string& valueToWrite);
    static std::string GenerateRandomHexValue();
};