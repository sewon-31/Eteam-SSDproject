#include "gmock/gmock.h"
#include "test_script2.h"
#include "ssd_interface.h"

#if _DEBUG

using namespace testing;

TEST(TestScript3Test, TestScript2) {
    std::ostringstream oss;
    auto oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    MockSSD ssd;
    std::string randomNum = "0x0000000A";
    testScript2 partialLbaWrite;

    // Write expectations
    EXPECT_CALL(ssd, write(_, _))
        .WillRepeatedly(Return());

    // Read expectations
    EXPECT_CALL(ssd, read(0))
        .WillRepeatedly(Return(randomNum));
    EXPECT_CALL(ssd, read(1))
        .WillRepeatedly(Return(randomNum));
    EXPECT_CALL(ssd, read(2))
        .WillRepeatedly(Return(randomNum));
    EXPECT_CALL(ssd, read(3))
        .WillRepeatedly(Return(randomNum));
    EXPECT_CALL(ssd, read(4))
        .WillRepeatedly(Return(randomNum));

    partialLbaWrite.run(ssd);
    // °ª ºñ±³	
    std::cout.rdbuf(oldCoutStreamBuf);
    EXPECT_EQ("PASS", oss.str());
}
#endif