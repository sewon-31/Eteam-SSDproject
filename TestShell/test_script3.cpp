#include "test_script3.h"
#include <random>
#include <cstdio>

void TestScript3::writeReadAging(SSDInterface& ssd) {
    for (int i = 0; i < 200; ++i) {
#if _DEBUG
        std::string val0 = "0x00001111";
        std::string val99 = "0x00009999";
#else
        std::string val0 = GenerateRandomHexValue();
        std::string val99 = GenerateRandomHexValue();
#endif
        ssd.write(0, val0);
        ssd.write(99, val99);

        if (!ReadCompare(ssd, 0, val0)) {
            std::cout << "FAIL";
            return;
        }

        if (!ReadCompare(ssd, 99, val99)) {
            std::cout << "FAIL";
            return;
        }
    }

    std::cout << "PASS";
}

bool TestScript3::ReadCompare(SSDInterface& ssd, int lba, const std::string& expectedValue) {
    std::string readValue = ssd.read(lba);
    if (readValue != expectedValue) {
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
