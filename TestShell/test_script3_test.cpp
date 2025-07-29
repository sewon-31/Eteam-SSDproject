#include "test_shell.h"
#include "gmock/gmock.h"
#include <random>

using namespace testing;

// 랜덤 헥사값 생성 유틸
std::string GenerateRandomHexValue() {
    static std::mt19937 rng{ std::random_device{}() };
    static std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

    char buffer[11];
    std::snprintf(buffer, sizeof(buffer), "0x%08X", dist(rng));
    return std::string(buffer);
}

TEST(TestScript3Test, WriteReadAgingTest) {
    MockSSD ssd;

    for (int i = 0; i < 200; ++i) {
        std::string val0 = GenerateRandomHexValue();
        std::string val99 = GenerateRandomHexValue();

        // Write expectations
        EXPECT_CALL(ssd, write(0, val0));
        EXPECT_CALL(ssd, write(99, val99));

        // Read expectations
        EXPECT_CALL(ssd, read(0)).WillOnce(Return(val0));
        EXPECT_CALL(ssd, read(99)).WillOnce(Return(val99));

        // 실행
        ssd.write(0, val0);
        ssd.write(99, val99);

        std::string read0 = ssd.read(0);
        std::string read99 = ssd.read(99);

        // 값 비교
        EXPECT_EQ(read0, val0);
        EXPECT_EQ(read99, val99);
    }
}
