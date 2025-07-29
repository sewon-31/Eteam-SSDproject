#include "ssd_interface.h"

class TestShell {
public:
	void setSSD(SSDInterface* ssd) {
		this->ssd = ssd;
	}
	string read(int lba);
private:
	SSDInterface* ssd;
};