#include "gmock/gmock.h"
#include "mock_ssd.h"
#include "command.h"
#include <string>
#include "common_test_fixture.h"

class WriteTestFixture : public testing::Test, public HandleConsoleOutputFixture {
public:
	const int VALID_LBA = 10;
	const int OVER_LBA = 100;
	const int UNDER_LBA = -1;
	MockSSD mockSSD;
	SSDDriver realSSD;
	MockSSDDriver mockSSDDriver;
	WriteCommand writeCmd{ &mockSSD };
	FullWriteCommand fullWriteCmd{ &mockSSD };
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
	void executeWrite(int lba, string value) {
		vector<string> args = { std::to_string(lba), value};
		writeCmd.execute(args);
	}
	void executeFullWrite(string value) {
		vector<string> args = { value};
		fullWriteCmd.execute(args);
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
	EXPECT_CALL(mockSSD, write(VALID_LBA, value)).Times(1);
	executeWrite(VALID_LBA, value);
	EXPECT_EQ(WRITE_DONE, getLastLine(oss.str()));
}

TEST_F(WriteTestFixture, TestWriteInvalidLBAOverUpperBound) {
	EXPECT_CALL(mockSSD, write).Times(0);
	executeWrite(OVER_LBA, value);
}

TEST_F(WriteTestFixture, TestWriteInvalidLBAUnderLowerBound) {
	EXPECT_CALL(mockSSD, write).Times(0);
	executeWrite(UNDER_LBA, value);
}
TEST_F(WriteTestFixture, TestInvalidSSD) {
	TestShell shell;
	vector<string> commandVector = { "write", std::to_string(VALID_LBA), value};
	EXPECT_CALL(mockSSD, write).Times(0);
	shell.ExecuteCommand(commandVector);
}

TEST_F(WriteTestFixture, TestFullWrite) {
	for (int i = 0; i < 100; i++)
		EXPECT_CALL(mockSSD, write(i, value)).Times(1);
	executeFullWrite(value);
	EXPECT_EQ(FULL_WRITE_DONE, getLastLine(oss.str()));
}

TEST_F(WriteTestFixture, TestRealSSDWriteFail) {
	if (isFileExists(SSD_EXE_FILE)) {
		GTEST_SKIP() << SSD_EXE_FILE << " not found, skipping test.";
	}
	TestShell shell{ &realSSD };
	vector<string> commandVector = { "write", std::to_string(VALID_LBA), value };
	shell.ExecuteCommand(commandVector);
	EXPECT_EQ(WRITE_FAIL, getLastLine(oss.str()));
}

TEST_F(WriteTestFixture, TestMockSSDDriverWrite) {
	TestShell shell{ &mockSSDDriver };
	vector<string> commandVector = { "write", std::to_string(VALID_LBA), value };
	EXPECT_CALL(mockSSDDriver, runExe)
		.WillRepeatedly(testing::Return(true));
	shell.ExecuteCommand(commandVector);
	EXPECT_EQ(WRITE_DONE, getLastLine(oss.str()));
}

TEST_F(WriteTestFixture, TestRealSSDWrite) {
	if (!isFileExists(SSD_EXE_FILE)) {
		GTEST_SKIP() << SSD_EXE_FILE << " not found, skipping test.";
	}
	TestShell shell{ &realSSD };
	vector<string> commandVector = { "write", std::to_string(VALID_LBA), value };
	shell.ExecuteCommand(commandVector);
	EXPECT_EQ(WRITE_DONE, getLastLine(oss.str()));

	std::ifstream nand("../ssd_nand.txt");
	ASSERT_TRUE(nand.is_open());
	std::string line;
	std::vector<string> outputData;
	while (std::getline(nand, line)) {
		outputData.push_back(line);
	}
	ASSERT_EQ(outputData.size(), 100);
	EXPECT_EQ(outputData.at(VALID_LBA), value);
}