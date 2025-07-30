#include "gmock/gmock.h"
#include "test_shell.h"
class WriteTestFixture : public testing::Test {
public:
	const int VALID_LBA = 10;
	const int OVER_LBA = 100;
	const int UNDER_LBA = -1;
	MockSSD mockSSD;
	SSDDriver realSSD;
	std::string value = "0x12345678";
	const std::string SSD_EXE_FILE = "./SSD.exe";
	bool isFileExists(const std::string& path) {
		std::ifstream file(path);
		return file.good();
	}
};
TEST_F(WriteTestFixture, TestBasicWrite) {
	TestShell shell{ &mockSSD };
	EXPECT_CALL(mockSSD, write(VALID_LBA, value)).Times(1);
	shell.write(VALID_LBA, value);
}

TEST_F(WriteTestFixture, TestWriteInvalidLBAOverUpperBound) {
	TestShell shell{ &mockSSD };
	std::string value = "0x12345678";
	EXPECT_CALL(mockSSD, write).Times(0);
	shell.write(OVER_LBA, value);
}

TEST_F(WriteTestFixture, TestWriteInvalidLBAUnderLowerBound) {
	TestShell shell{ &mockSSD };
	EXPECT_CALL(mockSSD, write).Times(0);
	shell.write(UNDER_LBA, value);
}

TEST_F(WriteTestFixture, TestInvalidSSD) {
	TestShell shell;
	shell.write(VALID_LBA, value);
	EXPECT_CALL(mockSSD, write).Times(0);
}

TEST_F(WriteTestFixture, TestFullWrite) {
	TestShell shell{ &mockSSD };
	for (int i = 0; i < 100; i++)
		EXPECT_CALL(mockSSD, write(i, value)).Times(1);
	shell.fullWrite(value);
}

TEST_F(WriteTestFixture, TestRealSSDWriteFail) {
	if (isFileExists(SSD_EXE_FILE)) {
		GTEST_SKIP() << SSD_EXE_FILE << " not found, skipping test.";
	}
	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());
	TestShell shell{ &realSSD };
	shell.write(VALID_LBA, value);
	std::cout.rdbuf(oldCoutStreamBuf);
	string expect = "[WRITE] Fail\n";
	EXPECT_EQ(expect, oss.str());
}
