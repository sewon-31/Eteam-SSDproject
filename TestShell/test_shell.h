#include "ssd_interface.h"

class TestShell {
public:
	TestShell () {}
	TestShell(SSDInterface* ssd) :ssd(ssd) {}
	void setSSD(SSDInterface* ssd) {
		this->ssd = ssd;
	}
	void read(int lba);
	void write(int lba, std::string value);
private:
	SSDInterface* ssd;
};
