#include "test_shell.h"


#if _DEBUG
using namespace testing;

int main() {
	InitGoogleMock();
	return RUN_ALL_TESTS();
}

#else

int main(int argc, char* argv[]) {
	TestShell ts(new SSDDriver());

	if (argc == 1) {
		ts.runShell();
	}

	if (argc > 1) {
		ts.runScript(argv[1]);
	}
	
	return 0;
}

#endif