#pragma once
#include "ssd_interface.h"

class TestScript1 {
public:
	TestScript1(SSDInterface& ssd)
		: ssd(ssd) {};
	void Run();

private:
	SSDInterface& ssd;
};