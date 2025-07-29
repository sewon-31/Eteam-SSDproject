#include "test_script1.h"

void FullWriteAndReadCompare::Run(void) {
	for (int startLba = 0; startLba < MAX_LBA; startLba += OPERATE_COUNT_PER_LOOP) {
		string data = getExpectData(startLba);
		// Write
		for (int offset = 0; offset < OPERATE_COUNT_PER_LOOP; offset++) {
			int lba = startLba + offset;
			ssd.write(lba, data);
		}

		// Read & Compare
		for (int offset = 0; offset < OPERATE_COUNT_PER_LOOP; offset++) {
			int lba = startLba + offset;
			string output = ssd.read(lba);

			if (output != data){
				cout << "Fail";
				return;
			}
		}
	}
	cout << "PASS";
}

string FullWriteAndReadCompare::getExpectData(int lba)
{
	int loopIdx = lba / OPERATE_COUNT_PER_LOOP;
	return inputArray[loopIdx];
}
