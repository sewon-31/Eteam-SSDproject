#include "test_shell.h"

using namespace testing;

class TestShellRead : public Test {
public:
	TestShell shell;
	MockSSD ssd;

	const string HEADER = "[Read] LBA ";
	const string MIDFIX = " : ";
	const string FOOTER = "\n";
	const string EXPECT_AA = "0xAAAAAAAA";

	void ssdReadFileSetUp() {
		std::string filePath = "ssd_output.txt";
		std::ofstream outfile(filePath);
		outfile << EXPECT_AA << std::endl;
		outfile.close();
	}
};

TEST_F(TestShellRead, ReadPass) {
	ssdReadFileSetUp();
	
	EXPECT_CALL(ssd, read(0))
		.Times(1)
		.WillRepeatedly(Return(EXPECT_AA));

	shell.setSSD(&ssd);

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	shell.read(0);
	std::cout.rdbuf(oldCoutStreamBuf);

	string expect = HEADER + "00" + MIDFIX + EXPECT_AA + FOOTER;
	EXPECT_EQ(expect, oss.str());
}

TEST_F(TestShellRead, FullReadPass) {
	ssdReadFileSetUp();

	EXPECT_CALL(ssd, read)
		.Times(100)
		.WillRepeatedly(Return(EXPECT_AA));

	shell.setSSD(&ssd);

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
