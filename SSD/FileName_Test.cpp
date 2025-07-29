#include "gmock/gmock.h"
#include "FileInterface.cpp"
#include <string>
#include <iostream>

using namespace testing;

class TCFixture :public testing::Test {
public:

};

TEST(FILEINTERFACE, TC_BASE) {

	EXPECT_EQ(1,1);
}

TEST(FILEINTERFACE, TC_FILE_OPEN) {
	FileInterface FI;
	EXPECT_TRUE(FI.fileOpen("ssd_nand.txt"));
	EXPECT_TRUE(FI.fileClose());
}

TEST(FILEINTERFACE, TC_FILE_OPEN_WRITE) {
	FileInterface FI;
	std::string expected_str = "12341234\n";
	std::string read_str;

	EXPECT_TRUE(FI.fileOpen("ssd_nand.txt"));
	EXPECT_TRUE(FI.fileWrite(expected_str));
	EXPECT_TRUE(FI.fileRead(read_str));
	EXPECT_EQ(read_str+"\n", expected_str);
	EXPECT_TRUE(FI.fileClose());
}

TEST(FILEINTERFACE, TC_FILE_OPEN_WRITE_MultiLine) {
	FileInterface FI;
	std::string expected_str = "12341234\n";
	std::string expected_str1 = "22341234\n";
	std::string expected_str2 = "32341234\n";
	std::string expected_str3 = "42341234\n";
	std::string read_str;

	EXPECT_TRUE(FI.fileOpen("ssd_nand.txt"));
	EXPECT_TRUE(FI.fileWrite(expected_str));
	EXPECT_TRUE(FI.fileWrite(expected_str1));
	EXPECT_TRUE(FI.fileWrite(expected_str2));
	EXPECT_TRUE(FI.fileWrite(expected_str3));
	EXPECT_TRUE(FI.fileRead(read_str));
	EXPECT_EQ(read_str + "\n", expected_str);
	EXPECT_TRUE(FI.fileRead(read_str));
	EXPECT_EQ(read_str + "\n", expected_str);
	EXPECT_TRUE(FI.fileRead(read_str));
	EXPECT_EQ(read_str + "\n", expected_str);
	EXPECT_TRUE(FI.fileRead(read_str));
	EXPECT_EQ(read_str + "\n", expected_str);

	EXPECT_TRUE(FI.fileClose());
}