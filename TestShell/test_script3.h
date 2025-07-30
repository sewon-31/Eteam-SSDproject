#pragma once
#include <string>
#include "test_shell.h"

class TestScript3 {
public:
    void writeReadAging(SSDInterface& ssd);
    bool ReadCompare(SSDInterface& ssd, int lba, const std::string& expectedValue);
    static std::string GenerateRandomHexValue();
};