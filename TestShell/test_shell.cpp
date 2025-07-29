#include "test_shell.h"
<<<<<<< HEAD

using std::cout;

void TestShell::read(int lba) {
	cout << ssd->read(lba);
}

void TestShell::write(int lba, std::string value) {
	ssd->write(lba, value);
}
