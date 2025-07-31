#include "gmock/gmock.h"
#include "ssd_command_parser.h"
#include "ssd.h"
#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace testing;

class MockParser : public SSDCommandParser {
public:
	MOCK_METHOD(void, setCommand, (const string& command), (override));
	MOCK_METHOD(bool, isValidCommand, (), (const, override));
	MOCK_METHOD(vector<string>, getCommandVector, (), (const, override));
};

class SSDTestFixture : public Test
{
protected:
	void SetUp() override
	{
		app.setParser(&mockParser);
	}
public:
	MockParser mockParser;
	SSD app;
	FileInterface& nandFile = app.getNandFile();
	FileInterface& outputFile = app.getOutputFile();
	TEST_CMD test_in;
	TEST_CMD test_out;
	TEST_CMD test_res;

	void processMockParserFunctions()
	{
		EXPECT_CALL(mockParser, setCommand)
			.Times(1);
		EXPECT_CALL(mockParser, isValidCommand)
			.WillRepeatedly(Return(true));
	}
};

TEST_F(SSDTestFixture, GetCommandTest) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "R", "0" }));

	app.run("R 0");

	EXPECT_EQ("R", app.parsedCommand.at(0));
	EXPECT_EQ("0", app.parsedCommand.at(1));
}

TEST_F(SSDTestFixture, ReadTest) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "R", "0" }));

	app.run("R 0");

	FileInterface nandFile = { "../ssd_nand.txt" };
	string expected;

	nandFile.fileOpen();
	if (nandFile.checkSize() >= 12)
		nandFile.fileReadOneline(expected);
	else
		expected = "0x00000000";
	nandFile.fileClose();

	EXPECT_EQ(expected, app.getData(0));
}

TEST_F(SSDTestFixture, GetInvalidCommandTest) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, isValidCommand)
		.WillRepeatedly(Return(false));

	EXPECT_CALL(mockParser, getCommandVector)
		.Times(0);

	app.run("R 0 0x00000000");
}

TEST_F(SSDTestFixture, WriteText) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "W", "0", "0x11112222"}));

	app.run("W 0 0x11112222");
	EXPECT_EQ("0x11112222", app.getData(0));

	app.clearData();
	EXPECT_EQ("0x00000000", app.getData(0));
}

TEST_F(SSDTestFixture, TC_FULL_WRITE) {
	string str[100];
	char buffer[16];

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.data[i] = str[i];
	}

	EXPECT_TRUE(app.writeNandFile());
	EXPECT_EQ(1200, nandFile.checkSize());
}

TEST_F(SSDTestFixture, TC_FULL_WRITE_READ) {
	string str[100];
	char buffer[16];
	char ret = true;

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.data[i] = str[i];
	}

	app.writeNandFile();
	EXPECT_EQ(1200, nandFile.checkSize());
	EXPECT_TRUE(app.readNandFile());

	for (int i = 0; i < 100; i++) {
		if (app.data[i] != str[i]) {
			ret = false;
			break;
		}
	}
	EXPECT_TRUE(ret);
}

TEST_F(SSDTestFixture, TC_WRITE_OUTPUT) {
	char ret = true;
	std::string expected_str = "0x12341234";

	EXPECT_TRUE(app.writeOutputFile(expected_str));
	EXPECT_EQ(12, outputFile.checkSize());
}

TEST_F(SSDTestFixture, TC_RUN_WRITE) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "W", "0", "0x11112222"}));

	app.run("W 0 0x11112222");

	FileInterface nandFile = { "../ssd_nand.txt" };

	string actual;
	nandFile.fileOpen();
	nandFile.fileReadOneline(actual);
	nandFile.fileClose();

	EXPECT_EQ(actual, "0x11112222");
}

TEST_F(SSDTestFixture, TC_RUN_READ) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "R", "0" }));

	app.run("R 0");

	FileInterface nandFile = { "../ssd_nand.txt" };

	string expected;
	nandFile.fileOpen();
	nandFile.fileReadOneline(expected);
	nandFile.fileClose();

	FileInterface outputFile = { "../ssd_output.txt" };

	string actual;
	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();

	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_EQ) {
	string op[6] = { "W","E","E","W","W","E" };
	int lba[6] = { 1,12,23,34,45,56 };
	int size[6] = { 1,2,3,1,1,6 };
	string data[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"0x00000000",
						"" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	
	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(6, ret);

	string op_res[6] = { "W","E","E","W","E","E" };
	int lba_res[6] = { 1,12,23,34,45,56 };
	int size_res[6] = { 1,2,3,1,1,6 };
	string data_res[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"",
						"" };
	int error = 0;
	for (int i = 0; i < ret; i++) {
		if (op_res[i] != test_out.op[i])	error++;
		if (lba_res[i] != test_out.lba[i])		error++;
		if (size_res[i] != test_out.size[i])	error++;
		if (data_res[i] != test_out.data[i])	error++;
	}
	EXPECT_EQ(0, error);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_REDUCE_1) {
	string op[6] = { "W","E","E","W","W","E" };
	int lba[6] = { 1,12,20,34,45,56 };
	int size[6] = { 1,9, 1,1,1,6 };
	string data[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"0x00000000",
						"" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(5, ret);

	string op_res[6] = { "W","E","E","W","E","E" };
	int lba_res[6] = { 1,12,23,34,45,56 };
	int size_res[6] = { 1,2,3,1,1,6 };
	string data_res[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"",
						"" };
	int error = 0;
	for (int i = 0; i < ret; i++) {
		if (op_res[i] != test_out.op[i])	error++;
		if (lba_res[i] != test_out.lba[i])		error++;
		if (size_res[i] != test_out.size[i])	error++;
		if (data_res[i] != test_out.data[i])	error++;
	}
	EXPECT_EQ(0, error);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_ERS_OVER_10) {
	string op[6] = { "W","E","W","E","E","N" };
	int lba[6] = { 1,12,34,45,56 ,0};
	int size[6] = { 1,9,1,1,6, 1 };
	string data[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"",
						"" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(6, ret);

	string op_res[6] = { "W","E","E","W","E","E" };
	int lba_res[6] = { 1,12,23,34,45,56 };
	int size_res[6] = { 1,2,3,1,1,6 };
	string data_res[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"",
						"" };
	int error = 0;
	for (int i = 0; i < ret; i++) {
		if (op_res[i] != test_out.op[i])	error++;
		if (lba_res[i] != test_out.lba[i])		error++;
		if (size_res[i] != test_out.size[i])	error++;
		if (data_res[i] != test_out.data[i])	error++;
	}
	EXPECT_EQ(0, error);
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

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(5, ret);

	string op_res[6] = { "W","E","E","W","E","E" };
	int lba_res[6] = { 1,12,23,34,45,56 };
	int size_res[6] = { 1,2,3,1,1,6 };
	string data_res[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"",
						"" };
	int error = 0;
	for (int i = 0; i < ret; i++) {
		if (op_res[i] != test_out.op[i])	error++;
		if (lba_res[i] != test_out.lba[i])		error++;
		if (size_res[i] != test_out.size[i])	error++;
		if (data_res[i] != test_out.data[i])	error++;
	}
	EXPECT_EQ(0, error);
}

TEST_F(SSDTestFixture, reduceCMD_TC1_ERS4_WR2_TO_ERS3_WR2) {
	string op[6] = { "E","W","E","E","W","E" };
	int lba[6] =   {  0,  4,  5, 15, 16, 17 };
	int size[6] =  {  4,  1, 10,  1,  1,  4 };
	string data[6] = { "",
						"0x00012300",
						"",
						"",
						"0x00000010",
						"" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(5, ret);

	string op_res[6] = { "W","E","E","W","E","E" };
	int lba_res[6] = { 1,12,23,34,45,56 };
	int size_res[6] = { 1,2,3,1,1,6 };
	string data_res[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"",
						"" };
	int error = 0;
	for (int i = 0; i < ret; i++) {
		if (op_res[i] != test_out.op[i])	error++;
		if (lba_res[i] != test_out.lba[i])		error++;
		if (size_res[i] != test_out.size[i])	error++;
		if (data_res[i] != test_out.data[i])	error++;
	}
	EXPECT_EQ(0, error);
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

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(4, ret);

	string op_res[6] = { "W","E","E","W","E","E" };
	int lba_res[6] = { 1,12,23,34,45,56 };
	int size_res[6] = { 1,2,3,1,1,6 };
	string data_res[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"",
						"" };
	int error = 0;
	for (int i = 0; i < ret; i++) {
		if (op_res[i] != test_out.op[i])	error++;
		if (lba_res[i] != test_out.lba[i])		error++;
		if (size_res[i] != test_out.size[i])	error++;
		if (data_res[i] != test_out.data[i])	error++;
	}
	EXPECT_EQ(0, error);
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

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = app.reduceCMDBuffer(test_in, test_out);
	EXPECT_EQ(3, ret);

	string op_res[6] = { "W","E","E","W","E","E" };
	int lba_res[6] = { 1,12,23,34,45,56 };
	int size_res[6] = { 1,2,3,1,1,6 };
	string data_res[6] = { "0x00012300",
						"",
						"",
						"0x00000100",
						"",
						"" };
	int error = 0;
	for (int i = 0; i < ret; i++) {
		if (op_res[i] != test_out.op[i])	error++;
		if (lba_res[i] != test_out.lba[i])		error++;
		if (size_res[i] != test_out.size[i])	error++;
		if (data_res[i] != test_out.data[i])	error++;
	}
	EXPECT_EQ(0, error);
}
#if 0
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

	EXPECT_EQ(2, app.reduceCMDBuffer(test_in, test_out));
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

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	EXPECT_EQ(2, app.reduceCMDBuffer(test_in, test_out));
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

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	EXPECT_EQ(5, app.reduceCMDBuffer(test_in, test_out));
}
#endif