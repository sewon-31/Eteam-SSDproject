#include "gmock/gmock.h"
#include "file_interface.h"

using namespace testing;
using std::string;
using std::vector;

class FileInterfaceFixture : public Test {
public:
	const string testFilePath = "../ssd_interface_test.txt";

	const string expected_str = "0x12341234";
	const string expected_str1 = "0x22341234";
	const string expected_str2 = "0x32341234";
	const string expected_str3 = "0x42341234";
	string read_str;
};


TEST_F(FileInterfaceFixture, TC_FILE_WRITE_READ_SINGLE_LINE) {
	EXPECT_TRUE(FileInterface::clearFile(testFilePath));
	EXPECT_TRUE(FileInterface::writeLine(testFilePath, expected_str));
	EXPECT_TRUE(FileInterface::readLine(testFilePath, read_str));
	EXPECT_EQ(read_str, expected_str);
}

TEST_F(FileInterfaceFixture, TC_FILE_WRITE_READ_MULTILINE) {
	EXPECT_TRUE(FileInterface::clearFile(testFilePath));

	EXPECT_TRUE(FileInterface::writeLine(testFilePath, expected_str));
	EXPECT_TRUE(FileInterface::writeLine(testFilePath, expected_str1));
	EXPECT_TRUE(FileInterface::writeLine(testFilePath, expected_str2));
	EXPECT_TRUE(FileInterface::writeLine(testFilePath, expected_str3));

	vector<string> lines;
	EXPECT_TRUE(FileInterface::readAllLines(testFilePath, lines));
	ASSERT_EQ(lines.size(), 4);
	EXPECT_EQ(lines[0], expected_str);
	EXPECT_EQ(lines[1], expected_str1);
	EXPECT_EQ(lines[2], expected_str2);
	EXPECT_EQ(lines[3], expected_str3);
}

TEST_F(FileInterfaceFixture, TC_FILE_CLEAR) {
	EXPECT_TRUE(FileInterface::writeLine(testFilePath, expected_str));
	EXPECT_TRUE(FileInterface::clearFile(testFilePath));

	vector<string> lines;
	EXPECT_TRUE(FileInterface::readAllLines(testFilePath, lines));
	EXPECT_TRUE(lines.empty());
}

TEST_F(FileInterfaceFixture, TC_FILE_SIZE_CHECK) {
	EXPECT_TRUE(FileInterface::clearFile(testFilePath));
	EXPECT_TRUE(FileInterface::writeLine(testFilePath, expected_str));
	EXPECT_EQ(FileInterface::getFileSize(testFilePath), expected_str.size() + 2); // +2 for \r\n (windows)

	EXPECT_TRUE(FileInterface::writeLine(testFilePath, expected_str));
	EXPECT_EQ(FileInterface::getFileSize(testFilePath), (expected_str.size() + 2) * 2);
}

TEST_F(FileInterfaceFixture, TC_FILE_EXIST_CHECK) {
	EXPECT_TRUE(FileInterface::clearFile(testFilePath));
	EXPECT_TRUE(FileInterface::fileExists(testFilePath));
}

TEST_F(FileInterfaceFixture, TC_FILE_RENAME) {
	std::string newPath = "../ssd_interface_test_renamed.txt";

	EXPECT_TRUE(FileInterface::clearFile(testFilePath));
	EXPECT_TRUE(FileInterface::renameFile(testFilePath, newPath));
	EXPECT_TRUE(FileInterface::fileExists(newPath));
	EXPECT_FALSE(FileInterface::fileExists(testFilePath));

	// Restore
	FileInterface::renameFile(newPath, testFilePath);
}

TEST_F(FileInterfaceFixture, TC_DIRECTORY_CREATE_AND_CLEAR) {
	std::string testDir = "../test_dir";

	if (FileInterface::directoryExists(testDir)) {
		FileInterface::clearDirectory(testDir);
		FileInterface::removeFile(testDir);
	}

	EXPECT_TRUE(FileInterface::createDirectory(testDir));
	EXPECT_TRUE(FileInterface::directoryExists(testDir));

	// create file inside
	std::string nestedFile = testDir + "/file.txt";
	EXPECT_TRUE(FileInterface::writeLine(nestedFile, expected_str));

	EXPECT_TRUE(FileInterface::clearDirectory(testDir));
	EXPECT_TRUE(FileInterface::directoryExists(testDir)); // dir still exists
}
