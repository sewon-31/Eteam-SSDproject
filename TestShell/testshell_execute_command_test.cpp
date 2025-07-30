
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
	MOCK_METHOD(void, write, (int lba, std::string value));
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

	EXPECT_CALL(app, read(_))
		.Times(1);

	app.ExecuteCommand("read");
}
TEST_F(TestShellCommandOperatorFixture, Write) {
	MockTestShell app(&mockSSD);

	EXPECT_CALL(app, write(_, _))
		.Times(1);

	app.ExecuteCommand("write");
}