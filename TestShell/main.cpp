#include "test_shell.h"
#include "test_script2.h"
using namespace testing;

#if _DEBUG

int main() {
	InitGoogleMock();
	return RUN_ALL_TESTS();
}

#else

int main() {
<<<<<<< HEAD
	TestShell ts;
	ts.runShell();
=======
	TestShell shell;
	SSDDriver ssd;

	shell.setSSD(&ssd);
	shell.runShell();

>>>>>>> b3e8193 ([feature] Add Assertion with exe)
	return 0;
}

#endif