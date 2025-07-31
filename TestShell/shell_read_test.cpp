#include "mock_ssd.h"
#include "command.h"
#include "common_test_fixture.h"

using namespace testing;

class TestShellRead : public Test, public HandleConsoleOutputFixture {
public:
	TestShell shell;
	MockSSD mockSSD;
	MockSSDDriver SSDwithMockRunExe;

	const string HEADER = "[Read] LBA ";
	const string MIDFIX = " : ";
	const string FOOTER = "\n";
	const string EXPECT_AA = "0xAAAAAAAA";

	void ssdReadFileSetUp() {
		std::string filePath = "../ssd_output.txt";
		std::ofstream outfile(filePath);
		outfile << EXPECT_AA << std::endl;
		outfile.close();
	}
};

TEST_F(TestShellRead, ReadPassWithMockSSD) {
	ssdReadFileSetUp();

	EXPECT_CALL(mockSSD, read(0))
		.Times(1)
		.WillRepeatedly(Return(EXPECT_AA));

	ReadCommand cmd{ &mockSSD };

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	vector<string> args = { std::to_string(0) };
	cmd.execute(args);
	std::cout.rdbuf(oldCoutStreamBuf);

	string expect = HEADER + "00" + MIDFIX + EXPECT_AA + FOOTER;
	EXPECT_EQ(expect, getLastLine(oss.str()));
}

TEST_F(TestShellRead, FullReadPassWithMockSSD) {
	ssdReadFileSetUp();

	EXPECT_CALL(mockSSD, read)
		.Times(100)
		.WillRepeatedly(Return(EXPECT_AA));

	shell.setSSD(&mockSSD);

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	shell.fullRead();
	std::cout.rdbuf(oldCoutStreamBuf);

	string expect = "";
	for (int i = 0; i < 100; i++) {
		std::ostringstream oss;
		oss << std::setw(2) << std::setfill('0') << i;

		expect += HEADER;
		expect += oss.str();
		expect += MIDFIX;
		expect += EXPECT_AA;
		expect += FOOTER;
	}

	EXPECT_EQ(expect, oss.str());
}


TEST(SSDDriverRead, ReadPassWithMockRunExe) {
	MockSSDDriver SSDwithMockRunExe;

	EXPECT_CALL(SSDwithMockRunExe, runExe)
		.Times(1)
		.WillRepeatedly(Return(true));

	EXPECT_EQ("0xAAAAAAAA", SSDwithMockRunExe.read(0));
}

TEST(SSDDriverRead, ReadFailWithMockRunExe) {
	MockSSDDriver SSDwithMockRunExe;

	EXPECT_CALL(SSDwithMockRunExe, runExe)
		.Times(1)
		.WillRepeatedly(Return(false));

	try {
		SSDwithMockRunExe.read(0);
		FAIL();
	}
	catch (std::runtime_error e) {
		EXPECT_EQ(std::string(e.what()), "There is no SSD.exe\n");
	}
}

TEST_F(TestShellRead, ReadPassWithMockRunExe) {

	EXPECT_CALL(SSDwithMockRunExe, runExe)
		.Times(1)
		.WillRepeatedly(Return(true));

	ReadCommand cmd{ &SSDwithMockRunExe };

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	vector<string> args = { std::to_string(0) };
	cmd.execute(args);
	std::cout.rdbuf(oldCoutStreamBuf);

	string expect = HEADER + "00" + MIDFIX + EXPECT_AA + FOOTER;
	EXPECT_EQ(expect, getLastLine(oss.str()));
}

TEST_F(TestShellRead, FullReadPassWithMockRunExe) {
	ssdReadFileSetUp();

	EXPECT_CALL(SSDwithMockRunExe, runExe)
		.Times(100)
		.WillRepeatedly(Return(true));

	shell.setSSD(&SSDwithMockRunExe);

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	shell.fullRead();
	std::cout.rdbuf(oldCoutStreamBuf);

	string expect = "";
	for (int i = 0; i < 100; i++) {
		std::ostringstream oss;
		oss << std::setw(2) << std::setfill('0') << i;

		expect += HEADER;
		expect += oss.str();
		expect += MIDFIX;
		expect += EXPECT_AA;
		expect += FOOTER;
	}

	EXPECT_EQ(expect, oss.str());
}

//TEST_F(TestShellRead, ReadFailWithMockRunExe) {
//
//	EXPECT_CALL(SSDwithMockRunExe, runExe)
//		.WillRepeatedly(Return(false));
//
//	shell.setSSD(&SSDwithMockRunExe);
//
//	std::ostringstream oss;
//	auto oldCoutStreamBuf = std::cout.rdbuf();
//	std::cout.rdbuf(oss.rdbuf());
//
//	shell.read(0);
//	std::cout.rdbuf(oldCoutStreamBuf);
//
//	EXPECT_EQ("There is no SSD.exe\n", oss.str());
//}
//
//TEST_F(TestShellRead, FullReadFailWithMockRunExe) {
//	ssdReadFileSetUp();
//
//	EXPECT_CALL(SSDwithMockRunExe, runExe)
//		.WillRepeatedly(Return(false));
//
//	shell.setSSD(&SSDwithMockRunExe);
//
//	std::ostringstream oss;
//	auto oldCoutStreamBuf = std::cout.rdbuf();
//	std::cout.rdbuf(oss.rdbuf());
//
//	shell.fullRead();
//	std::cout.rdbuf(oldCoutStreamBuf);
//
//	EXPECT_EQ("There is no SSD.exe\n", oss.str());
//}
