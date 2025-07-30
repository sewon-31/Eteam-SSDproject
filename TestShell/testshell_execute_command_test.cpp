
#include "gmock/gmock.h"
#include "test_shell.h"

#include <string>

using namespace testing;
using namespace std;

class MockTestShell : public TestShell {
public:
	MockTestShell(SSDInterface* ssd) :
		TestShell{ ssd } {
	}
	MOCK_METHOD(void, read, (int lba));
private:
};

class TestShellCommandOperatorFixture : public Test {
public:
	void SetUp() override {
	}
	NiceMock<MockSSD> mockSSD;
};
TEST_F(TestShellCommandOperatorFixture, Read) {
	MockTestShell app(&mockSSD);
	vector<string> commandVector = { "read", "3" };

	EXPECT_CALL(app, read(3))
		.Times(1);

	app.ExecuteCommand(commandVector);
}