#pragma once
#include "ssd_interface.h"
#include <vector>

using namespace std;

class FullWriteAndReadCompare {
public:
	FullWriteAndReadCompare(SSDInterface& ssd);
	void Run();
	string getExpectData(int lba);

	const static int MAX_LBA = 100;
	const static int OPERATE_COUNT_PER_LOOP = 5;
	const static int LOOP_COUNT = MAX_LBA / OPERATE_COUNT_PER_LOOP;
private:
	string IntToHexStringSprintf(int hex_value);

	SSDInterface& ssd;
	string inputData[LOOP_COUNT];
};