#include "gmock/gmock.h"
#include "test_script.h"
#include <string>
#include "mock_ssd.h"

using namespace testing;
using namespace std;

class TestScriptsFixture : public Test {
public:
	void makeTests(std::string scriptName) {
		if (scriptName == "FullWriteAndReadCompare") script = new ScriptsFullWriteAndReadCompare(&mockSSD);
		if (scriptName == "PartialLBAWrite") script = new ScriptsPartialLBAWrite(&mockSSD);
		if (scriptName == "WriteReadAging") script = new ScriptsWriteReadAging(&mockSSD);

	}
	NiceMock<MockSSD> mockSSD;
	string invalidData = "0xFFFFFFFF";
	string testData = "0x00012345";

	ScriptsCommand* script;
	ostringstream oss;
};

TEST_F(TestScriptsFixture, Success) {
	makeTests("FullWriteAndReadCompare");
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);
	EXPECT_CALL(mockSSD, read(_))
		.Times(100)
		.WillRepeatedly(Return(testData));
	
	script->run();

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("PASS\n", oss.str());
}

TEST_F(TestScriptsFixture, FailWithInvalidData) {
	makeTests("FullWriteAndReadCompare");
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(5);
	EXPECT_CALL(mockSSD, read(_))
		.WillRepeatedly(Return(invalidData));

	script->run();

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("FAIL\n", oss.str());
}

TEST_F(TestScriptsFixture, TestScript2) {
	makeTests("PartialLBAWrite");

	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	// Write expectations
	EXPECT_CALL(mockSSD, write(_, _))
		.WillRepeatedly(Return());

	// Read expectations
	EXPECT_CALL(mockSSD, read(_))
		.WillRepeatedly(Return(testData));

	script->run();

	// Act
	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("PASS\n", oss.str());
}

TEST_F(TestScriptsFixture, WriteReadAging_CallsExpectedSequence) {
	makeTests("WriteReadAging");

	// For check "PASS" output
	std::ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	std::string val0 = "0x00001111";
	std::string val99 = "0x00009999";

	EXPECT_CALL(mockSSD, write(0, val0))
		.Times(200);
	EXPECT_CALL(mockSSD, write(99, val99))
		.Times(200);
	EXPECT_CALL(mockSSD, read(0))
		.Times(200)
		.WillRepeatedly(Return(val0));
	EXPECT_CALL(mockSSD, read(99))
		.Times(200)
		.WillRepeatedly(Return(val99));

	// Act
	script->run();

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("PASS\n", oss.str());
}
