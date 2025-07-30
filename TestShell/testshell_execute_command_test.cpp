
#include "gmock/gmock.h"
#include "test_shell.h"

#include <string>

using namespace testing;
using namespace std;

class MockTestShell : public TestShell {
public:
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
	MockTestShell app;
};
TEST_F(TestShellCommandOperatorFixture, Read) {
	vector<string> commandVector = { "read", "3" };

	EXPECT_CALL(app, read(_))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, Write) {
	vector<string> commandVector = { "write", "3", "0xAAAAAAAA"};

	EXPECT_CALL(app, write(_, _))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, FullRead) {
	vector<string> commandVector = { "fullread" };

	EXPECT_CALL(app, fullRead)
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, FullWrite) {
	vector<string> commandVector = { "fullwrite", "0xAAAAAAAA"};

	EXPECT_CALL(app, fullWrite(_))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, Help) {
	vector<string> commandVector = { "help" };

	EXPECT_CALL(app, help)
		.Times(1);

	app.ExecuteCommand(commandVector);
}