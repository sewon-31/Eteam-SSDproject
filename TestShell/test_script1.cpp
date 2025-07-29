#include "test_script1.h"

void TestScript1::Run(void) {
	for (int LoopIdx = 0; LoopIdx < LOOP_COUNT; LoopIdx++) {
		string data = getExpectData(LoopIdx* OPERATE_COUNT_PER_LOOP);
		// Write
		for (int OperateIdx = 0; OperateIdx < OPERATE_COUNT_PER_LOOP; OperateIdx++) {
			int lba = LoopIdx * OPERATE_COUNT_PER_LOOP + OperateIdx;
			ssd.write(lba, data);
		}

		// Read & Compare
		for (int OperateIdx = 0; OperateIdx < OPERATE_COUNT_PER_LOOP; OperateIdx++) {
			int lba = LoopIdx * OPERATE_COUNT_PER_LOOP + OperateIdx;
			string output = ssd.read(lba);
			if (output != data){
				cout << "Fail";
				return;
			}
		}
	}
	cout << "PASS";
}

string TestScript1::getExpectData(int lba)
{
	int loopIdx = lba / OPERATE_COUNT_PER_LOOP;
	return inputArray[loopIdx];
}
