#include "gmock/gmock.h"
#include "file_interface.h"

using namespace testing;
using std::vector;

class FileInterfaceFixture :public testing::Test {
public:
	FileInterface fileInterface = { "ssd_interface.txt" };

	std::string expected_str = "0x12341234";
	std::string expected_str1 = "0x22341234";
	std::string expected_str2 = "0x32341234";
	std::string expected_str3 = "0x42341234";
	std::string read_str;
};

TEST_F(FileInterfaceFixture, TC_FILE_OPEN) {
	EXPECT_TRUE(fileInterface.fileOpen());
	fileInterface.fileClose();
}

TEST_F(FileInterfaceFixture, TC_FILE_MAKEFILE) {
	fileInterface.fileRemove();
	EXPECT_TRUE(fileInterface.fileOpen());
	fileInterface.fileClose();
}

TEST_F(FileInterfaceFixture, TC_FILE_WRITE_READ) {
	fileInterface.fileRemove();
	fileInterface.fileOpen();
	EXPECT_TRUE(fileInterface.fileWriteOneline(expected_str));
	EXPECT_TRUE(fileInterface.fileReadOneline(read_str));
	EXPECT_EQ(read_str, expected_str);
	fileInterface.fileClose();
}

TEST_F(FileInterfaceFixture, TC_FILE_WRITE_READ_MULTILINE) {
	fileInterface.fileRemove();
	fileInterface.fileOpen();
	fileInterface.fileWriteOneline(expected_str);
	fileInterface.fileWriteOneline(expected_str1);
	fileInterface.fileWriteOneline(expected_str2);
	fileInterface.fileWriteOneline(expected_str3);
	fileInterface.fileReadOneline(read_str);
	EXPECT_EQ(read_str, expected_str);
	fileInterface.fileReadOneline(read_str);
	EXPECT_EQ(read_str, expected_str1);
	fileInterface.fileReadOneline(read_str);
	EXPECT_EQ(read_str, expected_str2);
	fileInterface.fileReadOneline(read_str);
	EXPECT_EQ(read_str, expected_str3);
	fileInterface.fileClose();
}

TEST_F(FileInterfaceFixture, TC_FILE_REMOVE) {
	fileInterface.fileOpen();
	fileInterface.fileWriteOneline(expected_str);
	fileInterface.fileClose();

	EXPECT_TRUE(fileInterface.fileRemove());
	fileInterface.fileOpen();
	fileInterface.fileReadOneline(read_str);
	EXPECT_EQ(read_str, "");
}

TEST_F(FileInterfaceFixture, TC_FILE_CHECK_EMPTY_FILE) {
	fileInterface.fileRemove();
	EXPECT_EQ(0, fileInterface.checkSize());
}

TEST_F(FileInterfaceFixture, TC_FILE_CHECK_FILESIZE) {
	fileInterface.fileRemove();
	fileInterface.fileOpen();
	EXPECT_TRUE(fileInterface.fileWriteOneline(expected_str));
	fileInterface.fileClose();
	EXPECT_EQ(12, fileInterface.checkSize());
	fileInterface.fileOpen();
	EXPECT_TRUE(fileInterface.fileWriteOneline(expected_str));
	fileInterface.fileClose();
	EXPECT_EQ(24, fileInterface.checkSize());
	fileInterface.fileRemove();
}