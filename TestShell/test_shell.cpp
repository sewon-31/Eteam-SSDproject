#include "test_shell.h"

using std::cout;

void TestShell::read(int lba) {
	std::string content = ssd->read(lba);

	std::ostringstream oss;
	oss << std::setw(2) << std::setfill('0') << lba;

	cout << READ_HEADER << oss.str() << READ_MIDFIX << content << READ_FOOTER;
}

void TestShell::fullRead()
{
	for (int addr = 0; addr < MAX_LBA; addr++) {
		read(addr);
	}
}

void TestShell::write(int lba, std::string value) {
	if (ssd == nullptr) return;
	if (lba >= 100 || lba < 0)
		return;
	ssd->write(lba, value);
	std::cout << "[WRITE] Done" << std::endl;
}

void TestShell::fullWrite(std::string value) {
	for (int i = 0; i < 100; i++)
		ssd->write(i, value);
}