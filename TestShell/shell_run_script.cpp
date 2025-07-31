#include "test_shell.h"
#include "command.h"
#include "common_test_fixture.h"

class ShellRunScript : public testing::Test, public HandleConsoleOutputFixture {
public:
	bool isFileExists(const std::string& path) {
		std::ifstream file(path);
		return file.good();
	}
};

TEST_F(ShellRunScript, NoFile) {
	TestShell shell(new SSDDriver());

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	std::string argv = "AAAAAAAA";

	shell.runScript(argv);

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("[Error] Invalid File Name.\n", getLastLine(oss.str()));
}


TEST_F(ShellRunScript, ExistFileButWrongCommand) {
	TestShell shell(new SSDDriver());

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	std::string argv = "ssd_script.txt";
	std::ofstream outfile(argv);
	outfile << "read 1" << std::endl;
	outfile.close();

	shell.runScript(argv);

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("read 1   ___   Run ... FAIL!\n", getLastLine(oss.str()));
}

TEST_F(ShellRunScript, ExistFileAndGoodCommand) {
	if (!isFileExists("SSD.exe")) {
		GTEST_SKIP() << "SSD.exe not found, skipping test.";
	}

	TestShell shell(new SSDDriver());

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	std::string argv = "ssd_script.txt";
	std::ofstream outfile(argv);
	outfile << "2_PartialLBAWrite" << std::endl;
	outfile.close();

	shell.runScript(argv);

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("2_PartialLBAWrite   ___   Run ... PASS\n", getLastLine(oss.str()));
}
