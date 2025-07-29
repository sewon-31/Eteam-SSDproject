#include "gmock/gmock.h"
#include "FileInterface.cpp"
#include <string>
#include <iostream>

using namespace testing;

TEST(FILEINTERFACE, TC_BASE) {

	EXPECT_EQ(1,1);
}

TEST(FILEINTERFACE, TC_FILE_OPEN) {
	FileInterface FI;
	bool ret = FI.fileOpen("ssd_nand.txt");
	EXPECT_TRUE(ret);
}

TEST(FILEINTERFACE, TC_FILE_OPEN_WRITE) {
	FileInterface FI;
	std::string str_test = "Write dummy.txt";
	std::string str;

	bool ret = FI.fileOpen("ssd_nand.txt");
	EXPECT_TRUE(ret);
	ret = FI.fileWrite(str_test);
	EXPECT_TRUE(ret);
	ret = FI.fileRead(str);

	EXPECT_EQ(str, str_test);
	EXPECT_TRUE(ret);
}
