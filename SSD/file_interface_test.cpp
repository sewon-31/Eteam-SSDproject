#include "gmock/gmock.h"
#include "file_interface.cpp"
#include <string>
#include <iostream>

using namespace testing;

class FileInterfaceFixture :public testing::Test {
public:
	FileInterface fileInterface;
	std::string expected_str = "12341234\n";
	std::string expected_str1 = "22341234\n";
	std::string expected_str2 = "32341234\n";
	std::string expected_str3 = "42341234\n";
	std::string read_str;
};

TEST_F(FileInterfaceFixture, TC_FILE_OPEN) {
	EXPECT_TRUE(fileInterface.fileOpen("ssd_nand.txt"));
	EXPECT_TRUE(fileInterface.fileClose());
}

TEST_F(FileInterfaceFixture, TC_FILE_WRITE_READ) {
	EXPECT_TRUE(fileInterface.fileOpen("ssd_nand.txt"));
	EXPECT_TRUE(fileInterface.fileWrite(expected_str));
	EXPECT_TRUE(fileInterface.fileRead(read_str));
	EXPECT_EQ(read_str+"\n", expected_str);
	EXPECT_TRUE(fileInterface.fileClose());
}

TEST_F(FileInterfaceFixture, TC_FILE_WRITE_READ_MultiLine) {
	EXPECT_TRUE(fileInterface.fileOpen("ssd_nand.txt"));
	EXPECT_TRUE(fileInterface.fileWrite(expected_str));
	EXPECT_TRUE(fileInterface.fileWrite(expected_str1));
	EXPECT_TRUE(fileInterface.fileWrite(expected_str2));
	EXPECT_TRUE(fileInterface.fileWrite(expected_str3));
	EXPECT_TRUE(fileInterface.fileRead(read_str));
	EXPECT_EQ(read_str + "\n", expected_str);
	EXPECT_TRUE(fileInterface.fileRead(read_str));
	EXPECT_EQ(read_str + "\n", expected_str);
	EXPECT_TRUE(fileInterface.fileRead(read_str));
	EXPECT_EQ(read_str + "\n", expected_str);
	EXPECT_TRUE(fileInterface.fileRead(read_str));
	EXPECT_EQ(read_str + "\n", expected_str);

	EXPECT_TRUE(fileInterface.fileClose());
}

