#include "gmock/gmock.h"
#include "test_script1.h"
#include <string>

using namespace testing;
using namespace std;

class TestScript1Fixture : public Test {
public:
};

TEST_F(TestScript1Fixture, VerifyCallCount) {
	MockSSD mockSSD;
	TestScript1 script(mockSSD);

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);

	EXPECT_CALL(mockSSD, read(_))
		.Times(100)
		.WillRepeatedly(Return("0x12341234"));

	script.Run();
}