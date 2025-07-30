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

	TestShell ts;

	SSDDriver ssd;
	ts.setSSD(&ssd);

	ts.runShell();

	return 0;
}

#endif