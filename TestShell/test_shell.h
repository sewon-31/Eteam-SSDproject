#include "ssd_interface.h"

class TestShell {
public:
	TestShell () {}
	TestShell(SSDInterface* ssd) :ssd(ssd) {}
	void setSSD(SSDInterface* ssd) {
		this->ssd = ssd;
	}
	void read(int lba);
private:
	SSDInterface* ssd;
};