#include "test_shell.h"
<<<<<<< HEAD

using std::cout;

void TestShell::read(int lba) {
	cout << ssd->read(lba);
}

void TestShell::write(int lba, std::string value) {
	if (ssd == nullptr) return;
	if (lba >= 100 || lba<0)
		return;
	ssd->write(lba, value);
	std::cout << "[WRITE] Done" << std::endl;
}
