#include "gmock/gmock.h"
#include "file_interface.h"

using namespace testing;
using std::vector;

class FileInterfaceFixture :public testing::Test {
public:
	FileInterface fileInterface;

	std::string expected_str = "12341234";
	std::string expected_str1 = "22341234";
	std::string expected_str2 = "32341234";
	std::string expected_str3 = "42341234";
	std::string read_str;
};

TEST_F(FileInterfaceFixture, TC_FILE_OPEN) {
	EXPECT_TRUE(fileInterface.fileOpen("ssd_nand.txt"));
	fileInterface.fileClose();
}

TEST_F(FileInterfaceFixture, TC_FILE_WRITE_READ) {
	fileInterface.fileRemove("ssd_nand.txt");
	fileInterface.fileOpen("ssd_nand.txt");
	EXPECT_TRUE(fileInterface.fileWriteOneline(expected_str));
	EXPECT_TRUE(fileInterface.fileReadOneline(read_str));
	EXPECT_EQ(read_str, expected_str);
	fileInterface.fileClose();
}

TEST_F(FileInterfaceFixture, TC_FILE_WRITE_READ_MULTILINE) {
	fileInterface.fileRemove("ssd_nand.txt");
	fileInterface.fileOpen("ssd_nand.txt");
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

	fileInterface.fileOpen("ssd_nand.txt");
	fileInterface.fileWriteOneline(expected_str);
	fileInterface.fileClose();

	EXPECT_TRUE(fileInterface.fileRemove("ssd_nand.txt"));
	fileInterface.fileOpen("ssd_nand.txt");
	fileInterface.fileReadOneline(read_str);
	EXPECT_EQ(read_str, "");
}
