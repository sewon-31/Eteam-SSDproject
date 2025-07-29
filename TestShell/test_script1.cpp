#include "test_script1.h"

void TestScript1::Run(void) {
	string data = "0x12341234";

	for (int LoopIdx = 0; LoopIdx < LOOP_COUNT; LoopIdx++) {
		// Write
		for (int OperateIdx = 0; OperateIdx < OPERATE_COUNT_PER_LOOP; OperateIdx++) {
			int lba = LoopIdx * OPERATE_COUNT_PER_LOOP + OperateIdx;
			ssd.write(lba, data);
		}

		// Read & Compare
		for (int OperateIdx = 0; OperateIdx < OPERATE_COUNT_PER_LOOP; OperateIdx++) {
			int lba = LoopIdx * OPERATE_COUNT_PER_LOOP + OperateIdx;
			string output = ssd.read(lba);
			if (output != data)
				// Fail
				return;
		}
	}
}
