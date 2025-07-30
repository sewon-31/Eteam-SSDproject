#include "gmock/gmock.h"
#include "test_shell.h"
TEST(ShellTest, TestPrintHelp) {
	TestShell shell;
	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());
	shell.help();
	std::cout.rdbuf(oldCoutStreamBuf);
	string expect =
		"Team: Easiest\n"
		"Member: Sewon Joo, Dokyeong Kim, Nayoung Yoon, Seungah Lim, Jaeyeong Jeon, Insang Cho, Dooyeun Hwang\n\n"
		"Available commands:\n\n"
		"\twrite <address> <value>\n"
		"\t\tWrite a 32-bit value to the specified address.\n\n"
		"\tread <address>\n"
		"\t\tRead a 32-bit value from the specified decimal address.\n\n"
		"\tfullwrite <value>\n"
		"\t\tFill the entire memory region with the specified 32-bit hex value.\n\n"
		"\tfullread\n"
		"\t\tRead and display the entire memory region.\n\n"
		"\thelp\n"
		"\t\tShow this help message.\n\n"
		"\texit\n"
		"\t\tExit the program.\n\n"
		"Address / Value format:\n"
		"\t<address> : Decimal integer (e.g., 16, 255)\n"
		"\t<value>   : 32-bit hexadecimal number\n"
		"\t\tMust start with '0x'\n"
		"\t\tMust contain exactly 8 hex digits (0-9, A-F)\n"
		"\t\tExample: 0x12345678, 0xDEADBEEF\n\n";


	EXPECT_EQ(expect, oss.str());
}