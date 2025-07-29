#include "test_shell.h"

string TestShell::read(int lba) {
	return ssd->read(lba);
}