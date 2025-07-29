#include "gmock/gmock.h"
#include "test_shell.h"
TEST(ShellTest, TestBasicWrite) {
	MockSSD ssd;
	TestShell shell{ &ssd };
	int lba = 10;
	std::string value = "0x12345678";
	EXPECT_CALL(ssd, write(lba, value)).Times(1);
	shell.write(lba, value);
}

TEST(ShellTest, TestWriteInvalidLBA) {
	MockSSD ssd;
	TestShell shell{ &ssd };
	int lba = 100;
	std::string value = "0x12345678";
	EXPECT_CALL(ssd, write(lba, value)).Times(0);
	shell.write(lba, value);
}
