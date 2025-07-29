#include "test_shell.h"

using std::cout;

void TestShell::read(int lba) {
	cout << ssd->read(lba);
}