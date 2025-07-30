#include "gmock/gmock.h"
#include "test_script1.h"
#include <string>

using namespace testing;
using namespace std;

class TestScript1Fixture : public Test {
public:
	void SetUp() override {
		script = new FullWriteAndReadCompare(mockSSD);
	}
	NiceMock<MockSSD> mockSSD;
	string invalidData = "0xFFFFFFFF";
	FullWriteAndReadCompare* script;
	ostringstream oss;
};

TEST_F(TestScript1Fixture, Success) {
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());
	
	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);

	for (int lba = 0; lba < 100; lba++) {
		string expectData = script->getExpectData(lba);
		EXPECT_CALL(mockSSD, read(lba))
			.Times(1)
			.WillOnce(Return(expectData));
	}

	script->Run();

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("PASS", oss.str());
}

TEST_F(TestScript1Fixture, FailWithInvalidData) {
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());

	EXPECT_CALL(mockSSD, read(0))
		.WillOnce(Return(script->getExpectData(0)));
	EXPECT_CALL(mockSSD, read(1))
		.WillOnce(Return(invalidData));

	script->Run();

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("FAIL", oss.str());
}