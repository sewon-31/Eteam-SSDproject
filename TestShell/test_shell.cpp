#include "test_shell.h"

using std::cout;

void TestShell::read(int lba) {
	ssd->read(lba);

	std::ifstream file(SSD_READ_RESULT);
	std::string content;

	std::getline(file, content);

	cout << content;
	file.close();
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
