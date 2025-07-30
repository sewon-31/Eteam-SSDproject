#include "test_shell.h"

using namespace testing;

TEST(TestShellRead, ReadPass) {
	string expect = "0xAAAAAAAA";

	// unit test about file read
	std::string filePath = "ssd_output.txt";
	std::ofstream outfile(filePath);
	outfile << expect << std::endl;
	outfile.close();

	MockSSD ssd;
	EXPECT_CALL(ssd, read(0))
		.Times(1);

	TestShell shell;
	shell.setSSD(&ssd);

	// cout compare
	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	shell.read(0);
	std::cout.rdbuf(oldCoutStreamBuf); //복원


	EXPECT_EQ(expect, oss.str());
}

TEST(TestShellRead, FullReadPass) {
	string writeData = "0xAAAAAAAA";

	// unit test about file read
	std::string filePath = "ssd_output.txt";
	std::ofstream outfile(filePath);
	outfile << writeData << std::endl;
	outfile.close();

	MockSSD ssd;
	EXPECT_CALL(ssd, read)
		.Times(100);

	TestShell shell;
	shell.setSSD(&ssd);

	// cout compare
	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	shell.fullRead();
	std::cout.rdbuf(oldCoutStreamBuf); //복원

	string expect = "";
	for (int i = 0; i < 100; i++) {
		expect += writeData;
	}

	EXPECT_EQ(expect, oss.str());
}
