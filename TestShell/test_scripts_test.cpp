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
		if (scriptName == "EraseAndWriteAging") script = new ScriptsEraseAndWriteAging(&mockSSD);

	}
	NiceMock<MockSSDDriver> mockSSD;
	string invalidData = "0xFFFFFFFF";
	string testData = "0x00012345";
	string erasedData = "0x00000000";

	ScriptsCommand* script;
};

TEST_F(TestScriptsFixture, Success) {
	makeTests("FullWriteAndReadCompare");

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);
	EXPECT_CALL(mockSSD, read(_))
		.Times(100)
		.WillRepeatedly(Return(testData));
	
	EXPECT_TRUE(script->run());
}

TEST_F(TestScriptsFixture, FailWithInvalidData) {
	makeTests("FullWriteAndReadCompare");

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(5);
	EXPECT_CALL(mockSSD, read(_))
		.WillRepeatedly(Return(invalidData));

	EXPECT_FALSE(script->run());
}

TEST_F(TestScriptsFixture, TestScript2) {
	makeTests("PartialLBAWrite");

	// Write expectations
	EXPECT_CALL(mockSSD, write(_, _))
		.WillRepeatedly(Return());

	// Read expectations
	EXPECT_CALL(mockSSD, read(_))
		.WillRepeatedly(Return(testData));

	EXPECT_TRUE(script->run());
}

TEST_F(TestScriptsFixture, WriteReadAging_CallsExpectedSequence) {
	makeTests("WriteReadAging");

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

	EXPECT_TRUE(script->run());
}

TEST_F(TestScriptsFixture, EraseAndWriteAgingSuccess) {
	makeTests("EraseAndWriteAging");

	std::string writeValue = "0x00001111";
	std::string overWriteValue = "0x00009999";

	const int LOOP = 30;
	const int TOTAL_SSD_CALL_IN_LOOP = 48;
	const int TOTAL_READ = 3;

	EXPECT_CALL(mockSSD, erase(_, 3))
		.Times(1 + TOTAL_SSD_CALL_IN_LOOP * LOOP);
	EXPECT_CALL(mockSSD, write(_, writeValue))
		.Times(TOTAL_SSD_CALL_IN_LOOP * LOOP);
	EXPECT_CALL(mockSSD, write(_, overWriteValue))
		.Times(TOTAL_SSD_CALL_IN_LOOP * LOOP);
	EXPECT_CALL(mockSSD, read(_))
		.Times(TOTAL_SSD_CALL_IN_LOOP * TOTAL_READ * LOOP)
		.WillRepeatedly(Return(erasedData));

	EXPECT_TRUE(script->run());
}