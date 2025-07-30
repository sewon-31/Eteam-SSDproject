#include <random>

#include "test_script.h"

int ScriptsCommand::generateRandomIntValue() const
{
	static std::mt19937 rng{ std::random_device{}() };
	static std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

	return dist(rng);
}

std::string ScriptsCommand::intToHexString(int hexValue) const
{
#if _DEBUG
	return "0x00012345";
#else
	char buffer[11];
	sprintf_s(buffer, "0x%08X", hexValue);
	return std::string(buffer);
#endif
}

bool ScriptsCommand::readCompare(int address, std::string hexValue) const
{
	if (ssd->read(address) == hexValue) return true;
	return false;
}

void ScriptsFullWriteAndReadCompare::run(void) {
	for (int startLba = 0; startLba < MAX_LBA; startLba += OPERATE_COUNT_PER_LOOP) {
		string data = intToHexString(generateRandomIntValue());
		// Write
		for (int offset = 0; offset < OPERATE_COUNT_PER_LOOP; offset++) {
			int lba = startLba + offset;
			ssd->write(lba, data);
		}

		// Read & Compare
		for (int offset = 0; offset < OPERATE_COUNT_PER_LOOP; offset++) {
			int lba = startLba + offset;

			if (!readCompare(lba, data)) {
				std::cout << "FAIL";
				return;
			}
		}
	}
	std::cout << "PASS";
}
