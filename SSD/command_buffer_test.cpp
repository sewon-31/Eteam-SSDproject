#include "gmock/gmock.h"
#include "ssd_command_builder.h"
#include "ssd.h"
#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace testing;

struct TestCmdFormat {
	string op[6];
	int lba[6];
	int size[6];
	string data[6];
};

class CMDBufTestFixture : public Test
{
public:
	TestCmdFormat test_in;
	TestCmdFormat test_out;

	CommandBuffer& testCmdBuf = CommandBuffer::getInstance();

	int convertCmdTypeAndTest(TestCmdFormat in, TestCmdFormat& out) {
		MergeCmd testCmdIn, testCmdOut;

		// Convert ReduceCmd format
		for (int i = 0; i < 6; ++i) {
			if (in.op[i] == "E")
				testCmdIn.op.push_back(CmdType::ERASE);
			else if (in.op[i] == "W")
				testCmdIn.op.push_back(CmdType::WRITE);
			else
				continue;

			testCmdIn.lba.push_back(in.lba[i]);
			testCmdIn.size.push_back(in.size[i]);
			testCmdIn.data.push_back(in.data[i]);
		}

		std::cout << "=== Input Commands ===\n";
		for (int i = 0; i < testCmdIn.op.size(); ++i) {
			if (in.op[i] != "W" && in.op[i] != "E") break;
			std::cout << "[" << i << "] "
				<< "OP: " << in.op[i]
				<< ", LBA: " << in.lba[i]
				<< ", DATA: " << (in.op[i] == "W" ? in.data[i] : "-")
				<< ", SIZE: " << in.size[i]
				<< "\n";
		}
		std::cout << "=======================\n";

		// Run reduceCMDBuffer
		int newCMDCount = testCmdBuf.mergeCmdBuffer(testCmdIn, testCmdOut);

		// Convert TestCmdFormat format
		for (int i = 0; i < newCMDCount; ++i) {
			if (testCmdOut.op[i] == CmdType::ERASE)
				out.op[i] = "E";
			else if (testCmdOut.op[i] == CmdType::WRITE)
				out.op[i] = "W";
			else
				out.op[i] = "N";

			out.lba[i] = testCmdOut.lba[i];
			out.size[i] = testCmdOut.size[i];
			out.data[i] = testCmdOut.data[i];
		}

		std::cout << "=== Optimized Command Output ===\n";
		for (int i = 0; i < newCMDCount; ++i) {
			std::cout << "[" << i << "] "
				<< "OP: " << out.op[i]
				<< ", LBA: " << out.lba[i]
				<< ", DATA: " << (out.op[i] == "W" ? out.data[i] : "-")
				<< ", SIZE: " << out.size[i]
				<< "\n";
		}
		std::cout << "================================\n";

		return newCMDCount;
	}
};

TEST_F(CMDBufTestFixture, reduceCMD_REPLACE_W0_TO_E) {
	string op[6] = { "W","E","E","W","W","E" };
	int lba[6] = { 1,12,23,34,45,56 };
	int size[6] = { 1,2,3,1,1,6 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x00000000","0x67890123" };
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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(6, ret);

	string op_res[6] = { "E", "E", "E", "E", "W", "W" };
	int lba_res[6] = { 12, 23, 45, 56, 1, 34 };
	int size_res[6] = { 2, 3, 1, 6, 1, 1 };
	string data_res[6] = { "","","","","0x12345678","0x45678901" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_MERCE_SEQ_CMD) {
	string op[6] = { "W","E","E","W","W","E" };
	int lba[6] = { 1,12,20,34,45,56 };
	int size[6] = { 1,9, 1,1,1,6 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x00000000","0x67890123" };

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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(5, ret);

	string op_res[6] = { "E","E","E","W","W" };
	int lba_res[6] = { 12,45,56,1,34 };
	int size_res[6] = { 9, 1, 6, 1, 1 };
	string data_res[6] = { "","","","0x12345678","0x45678901" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_ERS_RANGE_OVER) {
	string op[6] = { "W","E","W","E","E","E" };
	int lba[6] = { 1,12,34,45,56 ,62 };
	int size[6] = { 1,9,1,1,6, 5 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(6, ret);

	string op_res[6] = { "E", "E", "E", "E", "W", "W" };
	int lba_res[6] = { 12, 45, 56, 66, 1, 34 };
	int size_res[6] = { 9, 1, 10, 1, 1, 1 };
	string data_res[6] = { "","","","","0x12345678","0x34567890" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_NULL_CHECK) {
	string op[6] = { "W","E","N","N","N","N" };
	int lba[6] = { 1,12,34,45,56 ,62 };
	int size[6] = { 1,9,1,1,6, 5 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };
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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(2, ret);

	string op_res[6] = { "E", "W" };
	int lba_res[6] = { 12, 1 };
	int size_res[6] = { 9, 1 };
	string data_res[6] = { "","0x12345678" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_ERS2_WR1_TO_ERS1_WR1) {
	string op[6] = { "W","E","W","E","W","E" };
	int lba[6] = { 1,10,15,16,45,56 };
	int size[6] = { 1,5, 1,3,1,6 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x00000000","0x67890123" };
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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(5, ret);

	string op_res[6] = { "E", "E", "E", "W", "W" };
	int lba_res[6] = { 10, 45, 56, 1, 15 };
	int size_res[6] = { 9, 1, 6, 1, 1 };
	string data_res[6] = { "","","","0x12345678","0x34567890","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_ERS4_WR2_TO_ERS3_WR2) {
	string op[6] = { "E","W","E","E","W","E" };
	int lba[6] = { 0,  4,  5, 15, 16, 17 };
	int size[6] = { 4,  1, 10,  1,  1,  4 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };
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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(5, ret);

	string op_res[6] = { "E", "E", "E", "W", "W" };
	int lba_res[6] = { 0, 10, 20, 4, 16 };
	int size_res[6] = { 10, 10, 1, 1, 1 };
	string data_res[6] = { "","","","0x23456789","0x56789012" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_ERS4_WR2_TO_ERS2_WR2) {
	string op[6] = { "E","W","E","E","W","E" };
	int lba[6] = { 0,  4,  5, 15, 16, 17 };
	int size[6] = { 4,  1, 10,  1,  1,  3 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(4, ret);

	string op_res[6] = { "E", "E", "W", "W" };
	int lba_res[6] = { 0, 10, 4, 16 };
	int size_res[6] = { 10, 10, 1, 1 };
	string data_res[6] = { "","","0x23456789","0x56789012","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_ERS4_WR2_TO_ERS2_WR1) {
	string op[6] = { "E","W","E","E","W","E" };
	int lba[6] = { 0,  4,  5, 15, 16, 17 };
	int size[6] = { 4,  1, 10,  1,  1,  3 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(4, ret);

	string op_res[6] = { "E", "E", "W", "W" };
	int lba_res[6] = { 0, 10, 4, 16 };
	int size_res[6] = { 10, 10, 1, 1 };
	string data_res[6] = { "","","0x23456789","0x56789012","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_EWNE) {
	string op[6] = { "E","W","W","E","E","E" };
	int lba[6] = { 1, 2, 3, 4, 5, 6 };
	int size[6] = { 1, 1, 1, 1, 1, 1 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(3, ret);

	string op_res[6] = { "E", "W", "W" };
	int lba_res[6] = { 1, 2, 3 };
	int size_res[6] = { 6, 1, 1 };
	string data_res[6] = { "","0x23456789","0x34567890","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_EWNE_2) {
	string op[6] = { "E","W","W","E","E","E" };
	int lba[6] = { 1, 2, 3, 4, 5, 6 };
	int size[6] = { 1, 1, 1, 1, 1, 6 };
	string data[6] = { "",
						"0x23456789",
						"0x34567890",
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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(4, ret);

	string op_res[6] = { "E", "E", "W", "W" };
	int lba_res[6] = { 1, 11, 2, 3 };
	int size_res[6] = { 10, 1, 1, 1 };
	string data_res[6] = { "","","0x23456789","0x34567890","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_EWNE_3) {
	string op[6] = { "E","W","W","E","W","E" };
	int lba[6] = { 1, 2, 3, 4, 20, 26 };
	int size[6] = { 1, 1, 1, 1, 1, 6 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

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

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(5, ret);

	string op_res[6] = { "E", "E", "W", "W", "W" };
	int lba_res[6] = { 1, 26, 2, 3, 20 };
	int size_res[6] = { 4, 6, 1, 1, 1 };
	string data_res[6] = { "","","0x23456789","0x34567890","0x56789012","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_00) {
	string op[6] = { "W","E","E","W","E","N" };
	int lba[6] = { 20, 15, 19, 22,  14,  0 };
	int size[6] = { 0, 10, 10, 0, 8, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(3, ret);

	string op_res[6] = { "E", "E", "W" };
	int lba_res[6] = { 14, 24, 22 };
	int size_res[6] = { 10, 5, 1 };
	string data_res[6] = { "","","0x45678901","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_01) {
	string op[6] = { "E","W","W" };
	int lba[6] = { 20, 15, 19 };
	int size[6] = { 9, 10, 10 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(3, ret);

	string op_res[6] = { "E", "W", "W" };
	int lba_res[6] = { 20, 15, 19 };
	int size_res[6] = { 9, 1, 1 };
	string data_res[6] = { "","0x23456789","0x34567890","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_02) {
	string op[6] = { "E","W","W","W","E","N" };
	int lba[6] = { 20, 15, 19, 22,  17,  0 };
	int size[6] = { 9, 10, 10, 0, 4, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(4, ret);

	string op_res[6] = { "E", "E", "W", "W" };
	int lba_res[6] = { 17, 27, 15, 22 };
	int size_res[6] = { 10, 2, 1, 1 };
	string data_res[6] = { "","","0x23456789","0x45678901","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_03) {
	string op[6] = { "E","W","W","W","E","N" };
	int lba[6] = { 20, 15, 19, 22,  16,  0 };
	int size[6] = { 9, 10, 10, 0, 6, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(4, ret);

	string op_res[6] = { "E", "E", "W", "W" };
	int lba_res[6] = { 16, 26, 15, 22 };
	int size_res[6] = { 10, 3, 1, 1 };
	string data_res[6] = { "","","0x23456789","0x45678901","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_04) {
	string op[6] = { "E","W","W","W","E","N" };
	int lba[6] = { 20, 15, 19, 22,  15,  0 };
	int size[6] = { 9, 10, 10, 0, 8, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(2, ret);

	string op_res[6] = { "E", "E" };
	int lba_res[6] = { 15, 25 };
	int size_res[6] = { 10, 4 };
	string data_res[6] = { "","","","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_05) {
	string op[6] = { "W","W","E","E","W","N" };
	int lba[6] = { 20, 30, 20, 30,  16,  0 };
	int size[6] = { 1, 1,  10, 5, 6, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(3, ret);

	string op_res[6] = { "E", "E", "W" };
	int lba_res[6] = { 20, 30, 16 };
	int size_res[6] = { 10, 5, 1 };
	string data_res[6] = { "","","0x56789012","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_06) {
	string op[6] = { "W","W","E","E","W","N" };
	int lba[6] = { 20, 30, 21, 30,  30,  0 };
	int size[6] = { 1, 1,  10, 2, 1, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(4, ret);

	string op_res[6] = { "E", "E", "W", "W" };
	int lba_res[6] = { 21, 31, 20, 30 };
	int size_res[6] = { 10, 1, 1, 1 };
	string data_res[6] = { "","","0x12345678","0x56789012","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_07) {
	string op[6] = { "W","W","E","E","W","N" };
	int lba[6] = { 21, 30, 21, 30,  30,  0 };
	int size[6] = { 1, 1,  10, 2, 1, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(3, ret);

	string op_res[6] = { "E", "E", "W" };
	int lba_res[6] = { 21, 31, 30 };
	int size_res[6] = { 10, 1, 1 };
	string data_res[6] = { "","","0x56789012","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_08) {
	string op[6] = { "W","W","E","E","W","N" };
	int lba[6] = { 30, 30, 21, 30,  30,  0 };
	int size[6] = { 1, 1,  10, 2, 1, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(3, ret);

	string op_res[6] = { "E", "E", "W" };
	int lba_res[6] = { 21, 31, 30 };
	int size_res[6] = { 10, 1, 1 };
	string data_res[6] = { "","","0x56789012","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_09) {
	string op[6] = { "W","W","E","E","W","N" };
	int lba[6] = { 30, 31, 30, 31, 30,  0 };
	int size[6] = { 1, 1,  10, 2, 1, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(2, ret);

	string op_res[6] = { "E", "W" };
	int lba_res[6] = { 31, 30 };
	int size_res[6] = { 9, 1 };
	string data_res[6] = { "","0x56789012","","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_10) {
	string op[6] = { "W","W","E","E","W","N" };
	int lba[6] = { 30, 30, 30, 30,  30,  0 };
	int size[6] = { 1, 1,  10, 2, 1, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(2, ret);

	string op_res[6] = { "E", "W" };
	int lba_res[6] = { 31, 30 };
	int size_res[6] = { 9, 1 };
	string data_res[6] = { "","0x56789012","","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_11) {
	string op[6] = { "W","W","E","E","W","N" };
	int lba[6] = { 32, 31, 30, 31, 31,  0 };
	int size[6] = { 1, 1,  10, 2, 1, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(2, ret);

	string op_res[6] = { "E", "W" };
	int lba_res[6] = { 30, 31 };
	int size_res[6] = { 10, 1 };
	string data_res[6] = { "","0x56789012","","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_12) {
	string op[6] = { "W","W","E","E","W","N" };
	int lba[6] = { 98, 99, 90, 99, 97,  0 };
	int size[6] = { 1, 1,  10, 1, 1, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(2, ret);

	string op_res[6] = { "E", "W" };
	int lba_res[6] = { 90, 97 };
	int size_res[6] = { 10, 1 };
	string data_res[6] = { "","0x56789012","","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_13) {
	string op[6] = { "W","W","E","E","N","N" };
	int lba[6] = { 98, 99, 90, 99, 0,  0 };
	int size[6] = { 1, 1,  10, 1, 0, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(1, ret);

	string op_res[6] = { "E" };
	int lba_res[6] = { 90 };
	int size_res[6] = { 10 };
	string data_res[6] = { "","","","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_BUGCASE2) {
	string op[6] = { "W","W","W" };
	int lba[6] = { 20, 21, 20, 0,  0,  0 };
	int size[6] = { 1, 1, 1, 0, 0, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(2, ret);

	string op_res[6] = { "W", "W" };
	int lba_res[6] = { 20, 21 };
	int size_res[6] = { 1, 1 };
	string data_res[6] = { "0x34567890","0x23456789","","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, reduceCMD_TC1_BUGCASE) {
	string op[6] = { "E","W","E","N","N","N" };
	int lba[6] = { 18, 21, 18, 0,  0,  0 };
	int size[6] = { 3, 1, 5, 0, 0, 0 };
	string data[6] = { "0x12345678","0x23456789","0x34567890","0x45678901","0x56789012","0x67890123" };

	for (int i = 0; i < 6; i++) {
		test_in.op[i] = op[i];
		test_in.lba[i] = lba[i];
		test_in.size[i] = size[i];
		test_in.data[i] = data[i];
	}

	int ret = convertCmdTypeAndTest(test_in, test_out);
	EXPECT_EQ(1, ret);

	string op_res[6] = { "E" };
	int lba_res[6] = { 18 };
	int size_res[6] = { 5 };
	string data_res[6] = { "","","","","","" };

	for (int i = 0; i < ret; i++) {
		EXPECT_EQ(op_res[i], test_out.op[i]);
		EXPECT_EQ(lba_res[i], test_out.lba[i]);
		EXPECT_EQ(size_res[i], test_out.size[i]);
		EXPECT_EQ(data_res[i], test_out.data[i]);
	}
}

TEST_F(CMDBufTestFixture, TestCommandBufferWriteAfterErase) {
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
	int ret = convertCmdTypeAndTest(test_in, test_out);
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