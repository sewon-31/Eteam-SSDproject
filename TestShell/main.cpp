#include "command_executor.h"
#include "test_shell.h"

#if _DEBUG
using namespace testing;

int main() {
	InitGoogleMock();
	return RUN_ALL_TESTS();
}

#else

int main(int argc, char* argv[]) {
	std::unique_ptr<ICommandExecutor> executor;

	if (argc == 1) {
		executor = std::make_unique<TestShell>();
	}

	if (argc > 1) {
		executor = std::make_unique<TestScript>(argv[1]);
	}

	executor->execute();

	return 0;
}

#endif