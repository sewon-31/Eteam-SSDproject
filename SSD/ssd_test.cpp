#include "gmock/gmock.h"
#include "ssd_command_builder.h"
#include "ssd.h"
#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace testing;

class MockCommand : public ICommand {
public:
	MOCK_METHOD(void, run, (string& result), (override));
	MOCK_METHOD(void, execute, (string& result), (override));
	MOCK_METHOD(CmdType, getCmdType, (), (const, override));
	MOCK_METHOD(int, getLBA, (), (const, override));
};

class MockBuilder : public SSDCommandBuilder {
public:
	MOCK_METHOD(void, setCommandVector, (vector<string> commandVector), (override));
	MOCK_METHOD(bool, isValidCommand, (), (const, override));
	MOCK_METHOD(vector<string>, getCommandVector, (), (const, override));
	MOCK_METHOD(std::shared_ptr<ICommand>, createCommand, (std::vector<std::string>), (override));
};

class SSDTestFixture : public Test
{
protected:
	void SetUp() override {
		mockCmd = std::make_shared<MockCommand>();
		mockBuilder = std::make_shared<MockBuilder>();
	}

	//void TearDown() override {
	//	app.clearBufferDirectory();
	//}

public:
	std::shared_ptr<MockCommand> mockCmd;
	std::shared_ptr<MockBuilder> mockBuilder;

	SSD app;

	FileInterface nandFile{ "../ssd_nand.txt" };
	FileInterface outputFile{ "../ssd_output.txt" };
	TEST_CMD test_in;
	TEST_CMD test_out;
	TEST_CMD test_res;

	void processMockBuilderFunctions()
	{
		app.setBuilder(mockBuilder);
		EXPECT_CALL(*mockBuilder, isValidCommand)
			.WillRepeatedly(Return(true));
	}
};

TEST_F(SSDTestFixture, reduceCMD_TC1_BUGCASE2) {
	string op[6] = { "W","W","W","N","N","N" };
	int lba[6] = { 20, 21, 20, 0,  0,  0 };
	int size[6] = { 1, 1, 1, 0, 0, 0 };
	string data[6] = { "0x10012300",
						"0x20012300",
						"0x30012300",
						"",
						"",
						"" };
	//  0- 9   N E W W E N N N N N
	// 10-19   N N N N N N N N N N
	// 20-29   W N N N N N E E E E
	// 30-39   E E N N N N N N N N
	// 40-49   N N N N N N N N N N
	// 50-59   N N N N N N N N N N
	// 60-69   N N N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(1, ret);
}
#if 0
TEST_F(SSDTestFixture, reduceCMD_TC1_BUGCASE) {
	string op[6] = { "E","W","E","N","N","N" };
	int lba[6] = { 18, 21, 18, 0,  0,  0 };
	int size[6] = { 3, 1, 5, 0, 0, 0 };
	string data[6] = { "",
						"0x00012300",
						"",
						"",
						"",
						"" };
	//  0- 9   N E W W E N N N N N
	// 10-19   N N N N N N N N N N
	// 20-29   W N N N N N E E E E
	// 30-39   E E N N N N N N N N
	// 40-49   N N N N N N N N N N
	// 50-59   N N N N N N N N N N
	// 60-69   N N N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(1, ret);
}

TEST_F(SSDTestFixture, RunCommandTest1) {
	processMockBuilderFunctions();

	std::vector<std::string> input = { "R", "0" };

	EXPECT_CALL(*mockBuilder, createCommand(input))
		.WillOnce(Return(mockCmd));

	// if READ or FLUSH, directly call run
	EXPECT_CALL(*mockCmd, getCmdType)
		.WillOnce(Return(CmdType::READ));

	EXPECT_CALL(*mockCmd, run)
		.Times(1);

	app.run(input);
	app.clearBufferAndDirectory();
}

// DISABLED - WriteCommand::getValue cannot be mocked
TEST_F(SSDTestFixture, DISABLED_RunCommandTest2) {
	processMockBuilderFunctions();

	std::vector<std::string> input = { "W", "0", "0x00001111" };

	EXPECT_CALL(*mockBuilder, createCommand(input))
		.WillOnce(Return(mockCmd));

	// if WRITE or ERASE, go to buffer
	EXPECT_CALL(*mockCmd, getCmdType)
		.WillRepeatedly(Return(CmdType::WRITE));

	EXPECT_CALL(*mockCmd, getLBA)
		.WillRepeatedly(Return(0));

	EXPECT_CALL(*mockCmd, run)
		.Times(0);

	app.run(input);
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, GetInvalidCommandTest) {
	processMockBuilderFunctions();

	std::vector<std::string> input = { "R", "0", "0x00000000" };

	EXPECT_CALL(*mockBuilder, isValidCommand)
		.WillRepeatedly(Return(false));

	EXPECT_CALL(*mockBuilder, createCommand(input))
		.WillOnce(Return(nullptr));

	app.run(input);

	string expected;
	outputFile.fileOpen();
	outputFile.fileReadOneline(expected);
	outputFile.fileClose();

	EXPECT_EQ(expected, "ERROR");
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, ReadTest) {
	app.run({ "R", "0" });

	// actual
	string actual;
	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();

	// expected
	string expected;
	nandFile.fileOpen();
	if (nandFile.checkSize() >= 12)
		nandFile.fileReadOneline(expected);
	else
		expected = "0x00000000";
	nandFile.fileClose();

	// storage
	string storageData = app.getData(0);

	EXPECT_EQ(expected, storageData);
	EXPECT_EQ(actual, storageData);
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, WriteText) {
	vector<string> input = { "W", "0", "0x11112222" };

	app.run(input);
	app.clearBuffer();	// in real case, it would be re-created

	// storage
	string storageData = app.getData(0);

	// expected
	string actual;
	nandFile.fileOpen();
	if (nandFile.checkSize() >= 12)
		nandFile.fileReadOneline(actual);
	else
		actual = "0x00000000";
	nandFile.fileClose();

	// buffer makes it false
	EXPECT_NE("0x11112222", actual);
	EXPECT_NE("0x11112222", storageData);

	app.run({ "R", "0" });
	app.clearBuffer();	// in real case, it would be re-created

	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();

	EXPECT_EQ("0x11112222", actual);

	app.clearData();
	EXPECT_EQ("0x00000000", app.getData(0));
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_FULL_WRITE) {
	string str[100];
	char buffer[16];

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
		app.clearBuffer();	// in real case, it would be re-created
	}

	EXPECT_EQ(1200, nandFile.checkSize());
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_FULL_WRITE_READ) {
	string str[100];
	char buffer[16];
	char ret = true;

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
		app.clearBuffer();	// in real case, it would be re-created
	}

	EXPECT_EQ(1200, nandFile.checkSize());

	// to update storage from nand file
	//app.run({ "R", "0" });

	string actual;
	for (int i = 0; i < 100; i++) {

		app.run({ "R", std::to_string(i) });
		app.clearBuffer();	// in real case, it would be re-created

		outputFile.fileOpen();
		outputFile.fileReadOneline(actual);
		outputFile.fileClose();

		EXPECT_EQ(actual, str[i]);
		//if (app.getData(i) != str[i]) {
		//	ret = false;
		//	break;
		//}
	}
	//EXPECT_TRUE(ret);
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_WRITE_OUTPUT) {
	char ret = true;
	std::string expected_str = "0x12341234";

	EXPECT_TRUE(app.updateOutputFile(expected_str));
	EXPECT_EQ(12, outputFile.checkSize());
}

TEST_F(SSDTestFixture, TC_ERASE) {
	string str[100];
	char buffer[16];
	char ret = true;

	nandFile.fileClear();

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
		app.clearBuffer();	// in real case, it would be re-created
	}

	EXPECT_EQ(1200, nandFile.checkSize());

	app.run({ "E", "0", "10" });
	app.clearBuffer();	// in real case, it would be re-created

	string actual;

	app.run({ "R", "9" });
	app.clearBuffer();	// in real case, it would be re-created

	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();

	EXPECT_EQ(actual, "0x00000000");

	app.run({ "R", "10" });
	app.clearBuffer();	// in real case, it would be re-created

	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();
	EXPECT_EQ(actual, str[10]);

	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, reduceCMD_REPLACE_W0_TO_E) {
	string op[6] = { "W","E","E","W","W","E" };
	int lba[6] = { 1,12,23,34,45,56 };
	int size[6] = { 1,2,3,1,1,6 };
	string data[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"0x00000000",
						"" };
	//  0- 9   N W N N N N N N N N
	// 10-19   N N E E N N N N N N
	// 20-29   N N N E E E N N N N
	// 30-39   N N N N W N N N N N
	// 40-49   N N N N N E N N N N
	// 50-59   N N N N N N E E E E
	// 60-69   E E N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}


	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(6, ret);
}

TEST_F(SSDTestFixture, reduceCMD_MERCE_SEQ_CMD) {
	string op[6] = { "W","E","E","W","W","E" };
	int lba[6] = { 1,12,20,34,45,56 };
	int size[6] = { 1,9, 1,1,1,6 };
	string data[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"0x00000000",
						"" };
	//  0- 9   N W N N N N N N N N
	// 10-19   N N E E E E E E E E
	// 20-29   E N N N N N N N N N
	// 30-39   N N N N W N N N N N
	// 40-49   N N N N N E N N N N
	// 50-59   N N N N N N E E E E
	// 60-69   E E N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(5, ret);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_ERS_RANGE_OVER) {
	string op[6] = { "W","E","W","E","E","E" };
	int lba[6] = { 1,12,34,45,56 ,62 };
	int size[6] = { 1,9,1,1,6, 5 };
	string data[6] = { "0x00012300",
						"",
						"0x00000100",
						"",
						"",
						"" };
	//  0- 9   N W N N N N N N N N
	// 10-19   N N E E E E E E E E
	// 20-29   E N N N N N N N N N
	// 30-39   N N N N W N N N N N
	// 40-49   N N N N N E N N N N
	// 50-59   N N N N N N E E E E
	// 60-69   E E E E E E E N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(6, ret);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_NULL_CHECK) {
	string op[6] = { "W","E","N","N","N","N" };
	int lba[6] = { 1,12,34,45,56 ,62 };
	int size[6] = { 1,9,1,1,6, 5 };
	string data[6] = { "0x00012300",
						"",
						"0x00000100",
						"",
						"",
						"" };
	//  0- 9   N W N N N N N N N N
	// 10-19   N N E E E E E E E E
	// 20-29   E N N N N N N N N N
	// 30-39   N N N N W N N N N N
	// 40-49   N N N N N E N N N N
	// 50-59   N N N N N N E E E E
	// 60-69   E E E E E E E N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(2, ret);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_ERS2_WR1_TO_ERS1_WR1) {
	string op[6] = { "W","E","W","E","W","E" };
	int lba[6] = { 1,10,15,16,45,56 };
	int size[6] = { 1,5, 1,3,1,6 };
	string data[6] = { "0x00012300",
						"",
						"0x00000100",
						"",
						"0x00000000",
						"" };
	//  0- 9   N W N N N N N N N N
	// 10-19   E E E E E W E E E N
	// 20-29   N N N N N N N N N N
	// 30-39   N N N N N N N N N N
	// 40-49   N N N N N E N N N N
	// 50-59   N N N N N N E E E E
	// 60-69   E E N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(5, ret);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_ERS4_WR2_TO_ERS3_WR2) {
	string op[6] = { "E","W","E","E","W","E" };
	int lba[6] = { 0,  4,  5, 15, 16, 17 };
	int size[6] = { 4,  1, 10,  1,  1,  4 };
	string data[6] = { "",
						"0x00012300",
						"",
						"",
						"0x00000010",
						"" };
	//  0- 9   E E E E W E E E E E
	// 10-19   E E E E E E W E E E
	// 20-29   E N N N N N N N N N
	// 30-39   N N N N N N N N N N
	// 40-49   N N N N N N N N N N
	// 50-59   N N N N N N N N N N
	// 60-69   N N N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(5, ret);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_ERS4_WR2_TO_ERS2_WR2) {
	string op[6] = { "E","W","E","E","W","E" };
	int lba[6] = { 0,  4,  5, 15, 16, 17 };
	int size[6] = { 4,  1, 10,  1,  1,  3 };
	string data[6] = { "",
						"0x00012300",
						"",
						"",
						"0x00000010",
						"" };
	//  0- 9   E E E E W E E E E E
	// 10-19   E E E E E E W E E E
	// 20-29   N N N N N N N N N N
	// 30-39   N N N N N N N N N N
	// 40-49   N N N N N N N N N N
	// 50-59   N N N N N N N N N N
	// 60-69   N N N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(4, ret);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_ERS4_WR2_TO_ERS2_WR1) {
	string op[6] = { "E","W","E","E","W","E" };
	int lba[6] = { 0,  4,  5, 15, 16, 17 };
	int size[6] = { 4,  1, 10,  1,  1,  3 };
	string data[6] = { "",
						"0x00012300",
						"",
						"",
						"0x00000000",
						"" };
	//  0- 9   E E E E W E E E E E
	// 10-19   E E E E E E E E E E
	// 20-29   N N N N N N N N N N
	// 30-39   N N N N N N N N N N
	// 40-49   N N N N N N N N N N
	// 50-59   N N N N N N N N N N
	// 60-69   N N N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(3, ret);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_EWNE) {
	string op[6] = { "E","W","W","E","E","E" };
	int lba[6] = { 1, 2, 3, 4, 5, 6 };
	int size[6] = { 1, 1, 1, 1, 1, 1 };
	string data[6] = { "",
						"0x00012300",
						"0x00012300",
						"",
						"",
						"" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(3, ret);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_EWNE_2) {
	string op[6] = { "E","W","W","E","E","E" };
	int lba[6] = { 1, 2, 3, 4, 5, 6 };
	int size[6] = { 1, 1, 1, 1, 1, 6 };
	string data[6] = { "",
						"0x00012300",
						"0x00012300",
						"",
						"",
						"" };
	//  0- 9   N E W W E E E E E E
	// 10-19   E E N N N N N N N N
	// 20-29   N N N N N N N N N N
	// 30-39   N N N N N N N N N N
	// 40-49   N N N N N N N N N N
	// 50-59   N N N N N N N N N N
	// 60-69   N N N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(4, ret);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_EWNE_3) {
	string op[6] = { "E","W","W","E","W","E" };
	int lba[6] = { 1, 2, 3, 4, 20, 26 };
	int size[6] = { 1, 1, 1, 1, 1, 6 };
	string data[6] = { "",
						"0x00012300",
						"0x00012300",
						"",
						"0x00012300",
						"" };
	//  0- 9   N E W W E N N N N N
	// 10-19   N N N N N N N N N N
	// 20-29   W N N N N N E E E E
	// 30-39   E E N N N N N N N N
	// 40-49   N N N N N N N N N N
	// 50-59   N N N N N N N N N N
	// 60-69   N N N N N N N N N N
	// 70-79   N N N N N N N N N N
	// 80-89   N N N N N N N N N N
	// 90-99   N N N N N N N N N N

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(5, ret);
}

TEST_F(SSDTestFixture, TestCommandBufferWriteAfterErase) {
	string op[6] = { "W","E" };
	int lba[6] = { 1, 1 };
	int size[6] = { 1,  1 };
	string data[6] = { "0x11111111", "" };
	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}
	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(1, ret);

	string op_res[6] = { "E" };
	int lba_res[6] = { 1 };
	int size_res[6] = { 1 };
	string data_res[6] = { "" };
	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

#endif