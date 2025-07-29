#include "gmock/gmock.h"
#include "FileInterface.cpp"

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
	bool ret = FI.fileOpen("ssd_nand.txt");
	EXPECT_TRUE(ret);
	ret = FI.fileWrite("write dummy");
	EXPECT_TRUE(ret);
}
