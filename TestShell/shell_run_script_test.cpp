#include "test_shell.h"
#include "command.h"
#include "common_test_fixture.h"

class ShellRunScriptTestFixture : public testing::Test, public HandleConsoleOutputFixture {
public:
	FileUtil fileUtil;
	std::ostringstream oss;

	void testRun(string argv) {
		ICommandExecutor* runScripts = new TestScript(argv);

		auto oldCoutStreamBuf = std::cout.rdbuf();
		std::cout.rdbuf(oss.rdbuf());

		runScripts->execute();

		std::cout.rdbuf(oldCoutStreamBuf);
	}
};

TEST_F(ShellRunScriptTestFixture, NoFile) {		
	std::string argv = "AAAAAAAA";

	testRun(argv);
	
	EXPECT_EQ("[Error] Invalid File Name.\n", getLastLine(oss.str()));
}


TEST_F(ShellRunScriptTestFixture, ExistFileButWrongCommand) {
	std::string argv = "ssd_script.txt";
	std::ofstream outfile(argv);
	outfile << "read 1" << std::endl;
	outfile.close();

	testRun(argv);

	EXPECT_EQ("read 1   ___   Run ... FAIL\n", getLastLine(oss.str()));
}

TEST_F(ShellRunScriptTestFixture, ExistFileAndGoodCommand) {
	if (fileUtil.fileExists("SSD.exe") == false) {
		GTEST_SKIP() << "SSD.exe not found, skipping test.";
	}

	std::string argv = "ssd_script.txt";
	std::ofstream outfile(argv);
	outfile << "2_PartialLBAWrite" << std::endl;
	outfile.close();

	testRun(argv);

	EXPECT_EQ("2_PartialLBAWrite   ___   Run ... PASS\n", getLastLine(oss.str()));
}