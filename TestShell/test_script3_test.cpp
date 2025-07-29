#include <gmock/gmock.h>
#include <unordered_map>
#include "test_script3.h"

using namespace testing;

TEST(TestScript3Test, WriteReadAging_CallsExpectedSequence) {
    // Arrange
    MockSSD mockSSD;
    TestScript3 testScript3;

    // For check "PASS" output
    std::ostringstream oss;
    auto oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    std::string val0 = "0x00001111";
    std::string val99 = "0x00009999";

    EXPECT_CALL(mockSSD, write(0, val0))
        .Times(200);
     EXPECT_CALL(mockSSD, write(99, val99))
        .Times(200);
    EXPECT_CALL(mockSSD, read(0))
        .Times(200)
        .WillRepeatedly(Return(val0));
    EXPECT_CALL(mockSSD, read(99))
        .Times(200)
        .WillRepeatedly(Return(val99));

    // Act
    testScript3.writeReadAging(mockSSD);

    std::cout.rdbuf(oldCoutStreamBuf);
    EXPECT_EQ("PASS", oss.str());
}
