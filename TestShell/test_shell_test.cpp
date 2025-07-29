#include "gmock/gmock.h"
#include "test_shell.h"
class WriteTestFixture : public testing::Test {
public:
	const int VALID_LBA = 10;
	const int OVER_LBA = 100;
	const int UNDER_LBA = -1;
	MockSSD ssd;
	std::string value = "0x12345678";
};
TEST_F(WriteTestFixture, TestBasicWrite) {
	TestShell shell{ &ssd };
	EXPECT_CALL(ssd, write(VALID_LBA, value)).Times(1);
	shell.write(VALID_LBA, value);
}

TEST_F(WriteTestFixture, TestWriteInvalidLBAOverUpperBound) {
	TestShell shell{ &ssd };
	std::string value = "0x12345678";
	EXPECT_CALL(ssd, write).Times(0);
	shell.write(OVER_LBA, value);
}

TEST_F(WriteTestFixture, TestWriteInvalidLBAUnderLowerBound) {
	TestShell shell{ &ssd };
	EXPECT_CALL(ssd, write).Times(0);
	shell.write(UNDER_LBA, value);
}

TEST_F(WriteTestFixture, TestInvalidSSD) {
	TestShell shell;
	shell.write(VALID_LBA, value);
	EXPECT_CALL(ssd, write).Times(0);
}
