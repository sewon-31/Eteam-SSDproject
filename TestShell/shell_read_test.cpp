#include "mock_ssd.h"
#include "command.h"
#include "common_test_fixture.h"

using namespace testing;

class ShellReadTestFixture : public Test, public HandleConsoleOutputFixture {
public:
	MockSSDDriver mockSSD;
	MockSSDDriverForRunExe ssdWithMockRunExe;
	TestShell shell;

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

TEST_F(ShellReadTestFixture, ReadPassWithMockSSD) {
	ssdReadFileSetUp();

	EXPECT_CALL(mockSSD, read(0))
		.Times(1)
		.WillRepeatedly(Return(EXPECT_AA));

	ReadCommand cmd{ &mockSSD };

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	vector<string> args = { std::to_string(0)};
	cmd.execute(args);
	std::cout.rdbuf(oldCoutStreamBuf);

	string expect = HEADER + "00" + MIDFIX + EXPECT_AA + FOOTER;
	EXPECT_EQ(expect, getLastLine(oss.str()));
}

TEST_F(ShellReadTestFixture, FullReadPassWithMockSSD) {
	ssdReadFileSetUp();

	EXPECT_CALL(mockSSD, read)
		.Times(100)
		.WillRepeatedly(Return(EXPECT_AA));


	FullReadCommand cmd{ &mockSSD };

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	vector<string> args = { };
	cmd.execute(args);
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

	EXPECT_EQ(expect, excludeFirstLine(oss.str()));
}

TEST_F(ShellReadTestFixture, SSDReadPassWithMockRunExe) {
	EXPECT_CALL(ssdWithMockRunExe, runExe)
		.Times(1)
		.WillRepeatedly(Return(true));

	EXPECT_EQ("0xAAAAAAAA", ssdWithMockRunExe.read(0));
}

TEST_F(ShellReadTestFixture, ShellReadFailWithMockRunExe) {
	EXPECT_CALL(ssdWithMockRunExe, runExe)
		.Times(1)
		.WillRepeatedly(Return(false));

	try {
		ssdWithMockRunExe.read(0);
		FAIL();
	}
	catch (std::runtime_error e) {
		EXPECT_EQ(std::string(e.what()), "Failed to execute ssd.exe for read()\n");
	}
}

TEST_F(ShellReadTestFixture, CMDReadPassWithMockRunExe) {
	EXPECT_CALL(ssdWithMockRunExe, runExe)
		.Times(1)
		.WillRepeatedly(Return(true));

	ReadCommand cmd{ &ssdWithMockRunExe };

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	vector<string> args = { std::to_string(0)};
	cmd.execute(args);
	std::cout.rdbuf(oldCoutStreamBuf);

	string expect = HEADER + "00" + MIDFIX + EXPECT_AA + FOOTER;
	EXPECT_EQ(expect, getLastLine(oss.str()));
}

TEST_F(ShellReadTestFixture, FullReadPassWithMockRunExe) {
	ssdReadFileSetUp();

	EXPECT_CALL(ssdWithMockRunExe, runExe)
		.Times(100)
		.WillRepeatedly(Return(true));

	FullReadCommand cmd{ &ssdWithMockRunExe };

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	vector<string> args = { };
	cmd.execute(args);
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

	EXPECT_EQ(expect, excludeFirstLine(oss.str()));
}

TEST_F(ShellReadTestFixture, CMDReadFailWithMockRunExe) {

	EXPECT_CALL(ssdWithMockRunExe, runExe)
		.WillRepeatedly(Return(false));

	ReadCommand cmd{ &ssdWithMockRunExe };

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	vector<string> args = {std::to_string(0)};
	cmd.execute(args);
	std::cout.rdbuf(oldCoutStreamBuf);

	EXPECT_EQ("Executing read from LBA 0\nFailed to execute ssd.exe for read()\n", oss.str());
}

TEST_F(ShellReadTestFixture, FullReadFailWithMockRunExe) {
	ssdReadFileSetUp();

	EXPECT_CALL(ssdWithMockRunExe, runExe)
		.WillRepeatedly(Return(false));

	FullReadCommand cmd{ &ssdWithMockRunExe };

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	vector<string> args = {};
	cmd.execute(args);
	std::cout.rdbuf(oldCoutStreamBuf);

	EXPECT_EQ("Executing fullread\nFailed to execute ssd.exe for read()\n", oss.str());
}
