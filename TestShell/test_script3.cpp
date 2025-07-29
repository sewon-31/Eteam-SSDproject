#include "test_script3.h"
#include <random>
#include <cstdio>

void TestScript3::writeReadAging(SSDInterface& ssd) {

    for (int i = 0; i < 200; ++i) {
        std::string val0 = TestScript3::GenerateRandomHexValue();
        std::string val99 = TestScript3::GenerateRandomHexValue();

        ReadCompare(ssd, 0, val0);
        ReadCompare(ssd, 99, val99);
    }
}

bool TestScript3::ReadCompare(SSDInterface& ssd, int lba, const std::string& valueToWrite) {
    // 1. Write
    ssd.write(lba, valueToWrite);

    // 2. Read
    std::string readValue = ssd.read(lba);

    // 3. Compare
    if (readValue != valueToWrite) {
        std::cout << "[FAIL] LBA: " << lba
            << ", Written: " << valueToWrite
            << ", Read: " << readValue << std::endl;
        return false;
    }

    return true;
}

std::string TestScript3::GenerateRandomHexValue() {
    static std::mt19937 rng{ std::random_device{}() };
    static std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

    char buffer[11];
    std::snprintf(buffer, sizeof(buffer), "0x%08X", dist(rng));
    return std::string(buffer);
}
