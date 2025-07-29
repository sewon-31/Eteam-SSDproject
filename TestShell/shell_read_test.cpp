#include "test_shell.h"

using namespace testing;

TEST(TestShellRead, ReadPass) {
	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	MockSSD ssd;
	EXPECT_CALL(ssd, read(0))
		.WillRepeatedly(testing::Return("0x00000000"));

	TestShell shell;
	shell.setSSD(&ssd);
	shell.read(0);

	std::cout.rdbuf(oldCoutStreamBuf); //º¹¿ø

	string expect = "0x00000000";
	EXPECT_EQ(expect, oss.str());
}
