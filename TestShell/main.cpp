#include "test_shell.h"

using namespace testing;

#if _DEBUG

int main() {
	InitGoogleMock();
	return RUN_ALL_TESTS();
}

#else

int main() {
	TestShell shell;

	shell.runShell();
	
	return 0;
}

#endif