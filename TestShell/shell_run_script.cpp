#include "test_shell.h"
#include "command.h"
#include "common_test_fixture.h"

class ShellRunScript : public testing::Test, public HandleConsoleOutputFixture {
public:
	FileUtil fileUtil;
	TestShell shell;
	std::ostringstream oss;

	void testRun(string argv) {
		shell.setSSD(new SSDDriver());
		std::cout.rdbuf(oss.rdbuf());

		shell.runScript(argv);
	}
};

TEST_F(ShellRunScript, NoFile) {		
	std::string argv = "AAAAAAAA";

	testRun(argv);
	
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("[Error] Invalid File Name.\n", getLastLine(oss.str()));
}


TEST_F(ShellRunScript, ExistFileButWrongCommand) {
	std::string argv = "ssd_script.txt";
	std::ofstream outfile(argv);
	outfile << "read 1" << std::endl;
	outfile.close();

	testRun(argv);

	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("read 1   ___   Run ... FAIL!\n", getLastLine(oss.str()));
}

TEST_F(ShellRunScript, ExistFileAndGoodCommand) {
	if (fileUtil.fileExists("SSD.exe") == false) {
		GTEST_SKIP() << "SSD.exe not found, skipping test.";
	}

	std::string argv = "ssd_script.txt";
	std::ofstream outfile(argv);
	outfile << "2_PartialLBAWrite" << std::endl;
	outfile.close();

	testRun(argv);

	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("2_PartialLBAWrite   ___   Run ... PASS\n", getLastLine(oss.str()));
}
