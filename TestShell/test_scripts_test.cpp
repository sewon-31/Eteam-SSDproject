#include "gmock/gmock.h"
#include "test_script.h"
#include <string>

using namespace testing;
using namespace std;

class TestScriptsFixture : public Test {
public:
	void SetUp() override {
		script = new ScriptsFullWriteAndReadCompare(&mockSSD);
	}
	NiceMock<MockSSD> mockSSD;
	string invalidData = "0xFFFFFFFF";
	string testData = "0x00012345";

	ScriptsFullWriteAndReadCompare* script;
	ostringstream oss;
};

TEST_F(TestScriptsFixture, Success) {
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);
	EXPECT_CALL(mockSSD, read(_))
		.Times(100)
		.WillRepeatedly(Return(testData));
	
	script->run();

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("PASS", oss.str());
}

TEST_F(TestScriptsFixture, FailWithInvalidData) {
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(5);
	EXPECT_CALL(mockSSD, read(_))
		.WillRepeatedly(Return(invalidData));

	script->run();

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("FAIL", oss.str());
}