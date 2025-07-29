#include "gmock/gmock.h"
#include "test_script1.h"
#include <string>

using namespace testing;
using namespace std;

class TestScript1Fixture : public Test {
public:
};

TEST_F(TestScript1Fixture, FullWriteAndReadCompareSuccess) {
	NiceMock<MockSSD> mockSSD;
	TestScript1 script(mockSSD);
	ostringstream oss;
	auto oldCoutStreamBuf = std::cout.rdbuf();
	std::cout.rdbuf(oss.rdbuf());
	int lba = 0;

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);

	for (int lba = 0; lba < 100; lba++) {
		string expectData = script.getExpectData(lba);
		EXPECT_CALL(mockSSD, read(lba))
			.Times(1)
			.WillOnce(Return(expectData));
	}

	script.Run();

	std::cout.rdbuf(oldCoutStreamBuf);
	EXPECT_EQ("PASS", oss.str());
}