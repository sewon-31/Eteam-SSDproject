#include "ssd_interface.h"

class TestShell {
public:
	TestShell() {}
	TestShell(SSDInterface* ssd) :ssd(ssd) {}
	void setSSD(SSDInterface* ssd) {
		this->ssd = ssd;
	}
	void read(int lba);
	void fullRead();
	void write(int lba, std::string value);
private:
	SSDInterface* ssd;
	const string SSD_READ_RESULT = "ssd_output.txt";
	const int MAX_LBA = 100;
	const string READ_HEADER = "[Read] LBA ";
	const string READ_MIDFIX = " : ";
	const string READ_FOOTER = "\n";
};
