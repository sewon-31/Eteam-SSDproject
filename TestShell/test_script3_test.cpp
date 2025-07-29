#include <gmock/gmock.h>
#include <unordered_map>
#include "test_script3.h"

using namespace testing;

TEST(TestScript3Test, WriteReadAging_CallsExpectedSequence) {
    // Arrange
    MockSSD mockSSD;
    TestScript3 testScript3;

    // Temporary storage to simulate SSD memory
    std::unordered_map<int, std::string> lbaMap;

    // Store value on write calls
    ON_CALL(mockSSD, write(_, _))
        .WillByDefault([&](int lba, const std::string& val) {
        lbaMap[lba] = val;
            });

    // Return stored value on read calls
    ON_CALL(mockSSD, read(_))
        .WillByDefault([&](int lba) {
        return lbaMap.count(lba) ? lbaMap[lba] : "0x00000000";
            });

    EXPECT_CALL(mockSSD, write(_, _)).Times(200 * 2);
    EXPECT_CALL(mockSSD, read(_)).Times(200 * 2);

    // Act
    testScript3.writeReadAging(mockSSD);
}
