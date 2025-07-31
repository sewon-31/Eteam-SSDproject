#include "file_util.h"
#include "gmock/gmock.h"

#include <cstdio>
#include <fstream>
#include <string>

class FileUtilTest : public ::testing::Test {
protected:
    std::string testDir = "test_dir";
    std::string testFile = "test_dir/test_file.txt";

    void SetUp() override {
        FileUtil::createDirectory(testDir);
        FileUtil::clearFile(testFile);
    }

    void TearDown() override {
        std::remove(testFile.c_str());
        _rmdir(testDir.c_str());
    }
};

TEST_F(FileUtilTest, DirectoryCreationAndExistence) {
    EXPECT_TRUE(FileUtil::directoryExists(testDir));
}

TEST_F(FileUtilTest, FileWriteAndExistence) {
    EXPECT_TRUE(FileUtil::writeLine(testFile, "Test line"));
    EXPECT_TRUE(FileUtil::fileExists(testFile));
}

TEST_F(FileUtilTest, FileReadLine) {
    FileUtil::writeLine(testFile, "Line to read");
    std::string readLine;
    EXPECT_TRUE(FileUtil::readLine(testFile, readLine));
    EXPECT_EQ(readLine, "Line to read");
}

TEST_F(FileUtilTest, FileClear) {
    FileUtil::writeLine(testFile, "Line1");
    FileUtil::clearFile(testFile);
    std::ifstream ifs(testFile.c_str());
    std::string content;
    std::getline(ifs, content);
    EXPECT_TRUE(content.empty());
}

TEST_F(FileUtilTest, ReadLineFailsOnEmptyFile) {
    FileUtil::clearFile(testFile);
    std::string line;
    bool result = FileUtil::readLine(testFile, line);
    EXPECT_FALSE(result);
}

TEST_F(FileUtilTest, WriteAndReadAllLines) {
    std::vector<std::string> inputLines = {
        "Line 1", "Line 2", "Line 3"
    };

    ASSERT_TRUE(FileUtil::writeAllLines(testFile, inputLines));

    std::vector<std::string> outputLines;
    ASSERT_TRUE(FileUtil::readAllLines(testFile, outputLines));

    ASSERT_EQ(inputLines.size(), outputLines.size());
    for (size_t i = 0; i < inputLines.size(); ++i) {
        EXPECT_EQ(inputLines[i], outputLines[i]);
    }
}

TEST_F(FileUtilTest, AppendLines) {
    std::vector<std::string> firstLines = { "A", "B" };
    std::vector<std::string> secondLines = { "C", "D" };

    ASSERT_TRUE(FileUtil::writeAllLines(testFile, firstLines, /*append=*/true));
    ASSERT_TRUE(FileUtil::writeAllLines(testFile, secondLines, /*append=*/true));

    std::vector<std::string> resultLines;
    ASSERT_TRUE(FileUtil::readAllLines(testFile, resultLines));

    std::vector<std::string> expected = { "A", "B", "C", "D" };
    ASSERT_EQ(resultLines, expected);
}
