#include <gmock/gmock.h>
#include "test_script3.h"

using namespace testing;

TEST(TestScript3Test, WriteReadAging_PerformsCorrectly) {
    MockSSD mockSSD;

    for (int i = 0; i < 200; ++i) {
        std::string val0 = TestScript3::GenerateRandomHexValue();
        std::string val99 = TestScript3::GenerateRandomHexValue();

        EXPECT_CALL(mockSSD, write(0, val0));
        EXPECT_CALL(mockSSD, write(99, val99));

        EXPECT_CALL(mockSSD, read(0)).WillOnce(Return(val0));
        EXPECT_CALL(mockSSD, read(99)).WillOnce(Return(val99));

        TestScript3::ReadCompare(mockSSD, 0, val0);
        TestScript3::ReadCompare(mockSSD, 99, val99);
    }
}