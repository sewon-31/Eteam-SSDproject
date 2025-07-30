#include "gmock/gmock.h"
#include "test_shell.h"
class WriteTestFixture : public testing::Test {
public:
	const int VALID_LBA = 10;
	const int OVER_LBA = 100;
	const int UNDER_LBA = -1;
	MockSSD mockSSD;
	SSDDriver realSSD;
	MockSSDDriver mockSSDDriver;
	std::string value = "0x12345678";
	std::ostringstream oss;
	std::streambuf* oldCoutStreamBuf;
	const std::string SSD_EXE_FILE = "./SSD.exe";
	const std::string WRITE_DONE = "[WRITE] Done\n";
	const std::string WRITE_FAIL = "[WRITE] Fail\n";
	const std::string FULL_WRITE_DONE = "[FULL_WRITE] Done\n";
	const std::string FULL_WRITE_FAIL = "[FULL_WRITE] Fail\n";
	bool isFileExists(const std::string& path) {
		std::ifstream file(path);
		return file.good();
	}
protected:
	void SetUp() override {
		oldCoutStreamBuf = std::cout.rdbuf();
		std::cout.rdbuf(oss.rdbuf());
	}
	void TearDown() override {
		std::cout.rdbuf(oldCoutStreamBuf);
	}
};
TEST_F(WriteTestFixture, TestBasicWrite) {
	TestShell shell{ &mockSSD };
	EXPECT_CALL(mockSSD, write(VALID_LBA, value)).Times(1);
	shell.write(VALID_LBA, value);
	EXPECT_EQ(WRITE_DONE, oss.str());
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
	EXPECT_EQ(FULL_WRITE_DONE, oss.str());
}

TEST_F(WriteTestFixture, TestRealSSDWriteFail) {
	if (isFileExists(SSD_EXE_FILE)) {
		GTEST_SKIP() << SSD_EXE_FILE << " not found, skipping test.";
	}
	TestShell shell{ &realSSD };
	shell.write(VALID_LBA, value);
	EXPECT_EQ(WRITE_FAIL, oss.str());
}

TEST_F(WriteTestFixture, TestMockSSDDriverWrite) {
	EXPECT_CALL(mockSSDDriver, runExe)
		.WillRepeatedly(testing::Return(true));
	TestShell shell{ &mockSSDDriver };
	shell.write(VALID_LBA, value);
	EXPECT_EQ(WRITE_DONE, oss.str());
}

TEST_F(WriteTestFixture, TestRealSSDWrite) {
	if (!isFileExists(SSD_EXE_FILE)) {
		GTEST_SKIP() << SSD_EXE_FILE << " not found, skipping test.";
	}
	TestShell shell{ &realSSD };
	shell.write(VALID_LBA, value);
	EXPECT_EQ(WRITE_DONE, oss.str());
	std::ifstream nand("ssd_nand.txt");
	ASSERT_TRUE(nand.is_open());
	std::string line;
	std::vector<string> outputData;
	while (std::getline(nand, line)) {
		outputData.push_back(line);
	}
	ASSERT_EQ(outputData.size(), 100);
	EXPECT_EQ(outputData.at(VALID_LBA), value);
}
