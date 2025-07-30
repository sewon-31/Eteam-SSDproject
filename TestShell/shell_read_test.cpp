#include "test_shell.h"

using namespace testing;

class TestShellRead : public Test {
public:
	TestShell shell;
	MockSSD ssd;
	string EXPECT_AA = "0xAAAAAAAA";

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
		.Times(1);

	shell.setSSD(&ssd);

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	shell.read(0);
	std::cout.rdbuf(oldCoutStreamBuf); //복원


	EXPECT_EQ(EXPECT_AA, oss.str());
}

TEST_F(TestShellRead, FullReadPass) {
	ssdReadFileSetUp();

	EXPECT_CALL(ssd, read)
		.Times(100);

	shell.setSSD(&ssd);

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	shell.fullRead();
	std::cout.rdbuf(oldCoutStreamBuf); //복원

	string expect = "";
	for (int i = 0; i < 100; i++) {
		expect += EXPECT_AA;
	}

	EXPECT_EQ(expect, oss.str());
}
