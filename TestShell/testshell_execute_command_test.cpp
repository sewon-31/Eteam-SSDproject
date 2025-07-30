
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
	MOCK_METHOD(void, fullRead, ());
	MOCK_METHOD(void, fullWrite, (std::string value));
	MOCK_METHOD(void, help, ());
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

	EXPECT_CALL(app, read(_))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, Write) {
	MockTestShell app(&mockSSD);
	vector<string> commandVector = { "write", "3", "0xAAAAAAAA"};

	EXPECT_CALL(app, write(_, _))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, FullRead) {
	MockTestShell app(&mockSSD);
	vector<string> commandVector = { "fullread" };

	EXPECT_CALL(app, fullRead)
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, FullWrite) {
	MockTestShell app(&mockSSD);
	vector<string> commandVector = { "fullwrite", "0xAAAAAAAA"};

	EXPECT_CALL(app, fullWrite(_))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, Help) {
	MockTestShell app(&mockSSD);
	vector<string> commandVector = { "help" };

	EXPECT_CALL(app, help)
		.Times(1);

	app.ExecuteCommand(commandVector);
}