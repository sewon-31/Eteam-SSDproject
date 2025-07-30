#include "test_script1.h"
#include <string>


FullWriteAndReadCompare::FullWriteAndReadCompare(SSDInterface& ssd)
	: ssd(ssd) {
	for (int LoopIdx = 0; LoopIdx < LOOP_COUNT; LoopIdx++) {
		inputData[LoopIdx] = IntToHexStringSprintf(LoopIdx);
	}
};
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
				cout << "FAIL";
				return;
			}
		}
	}
	cout << "PASS";
}

string FullWriteAndReadCompare::getExpectData(int lba)
{
	int loopIdx = lba / OPERATE_COUNT_PER_LOOP;
	return inputData[loopIdx];
}
string FullWriteAndReadCompare::IntToHexStringSprintf(int hex_value) {
	char buffer[11];
	sprintf_s(buffer, "0x%08X", hex_value);
	return std::string(buffer);
}
