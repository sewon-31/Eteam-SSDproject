#pragma once
#include "ssd_interface.h"
#include <vector>

using namespace std;

class TestScript1 {
public:
	TestScript1(SSDInterface& ssd)
		: ssd(ssd) {};
	void Run();
	string getExpectData(int loopIdx);

	const static int MAX_LBA = 100;
	const static int OPERATE_COUNT_PER_LOOP = 5;
	const static int LOOP_COUNT = MAX_LBA / OPERATE_COUNT_PER_LOOP;
private:
	SSDInterface& ssd;
	string inputArray[LOOP_COUNT];
};