#include "test_shell.h"


#if _DEBUG
using namespace testing;

int main() {
	InitGoogleMock();
	return RUN_ALL_TESTS();
}

#else

int main() {
	TestShell ts;
	ts.runShell();
	return 0;
}

#endif