#include <random>

#include "test_script.h"
#include "logger.h"

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
	auto readData = ssd->read(address);
	if (readData == hexValue) return true;
	PRINT_LOG("Read Compare Fail! lba: %d, exp: %s, actual: %s", address, hexValue, readData);
	return false;
}

bool ScriptsFullWriteAndReadCompare::run(void)
{
	PRINT_LOG("start");
	try {
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
					return false;
				}
			}
		}
		PRINT_LOG("Done");
		return true;
	}
	catch (...) { 
		PRINT_LOG("Fail! (exception)");
		return false; 
	}
}

bool ScriptsPartialLBAWrite::run()
{
	PRINT_LOG("start");
	try {
		for (int i = 0; i < 30; i++) {
			string data = intToHexString(generateRandomIntValue());

			ssd->write(4, data);
			ssd->write(0, data);
			ssd->write(3, data);
			ssd->write(1, data);
			ssd->write(2, data);

			for (int j = 0; j < 4; j++) {
				if (!readCompare(j, data)) {
					return false;
				}
			}
		}
		PRINT_LOG("Done");
		return true;
	}
	catch (...) {
		PRINT_LOG("Fail! (exception)");
		return false; 
	}
}

bool ScriptsWriteReadAging::run() 
{
	PRINT_LOG("start");
	try {
		for (int i = 0; i < 200; ++i) {
#if _DEBUG
			std::string val0 = "0x00001111";
			std::string val99 = "0x00009999";
#else
			std::string val0 = intToHexString(generateRandomIntValue());
			std::string val99 = intToHexString(generateRandomIntValue());
#endif
			ssd->write(0, val0);
			ssd->write(99, val99);

			if (!readCompare(0, val0)) {
				return false;
			}

			if (!readCompare(99, val99)) {
				return false;
			}
		}
		PRINT_LOG("Done");
		return true;
	}
	catch (...) {
		PRINT_LOG("Fail! (exception)");
		return false;
	}
}

bool ScriptsEraseAndWriteAging::run()
{
	PRINT_LOG("start");
	try {
		ssd->erase(0, 3);

		for (int i = 0; i < 30; ++i) {
#if _DEBUG
			std::string writeValue = "0x00001111";
			std::string overWriteValue = "0x00009999";
#else
			std::string writeValue = intToHexString(generateRandomIntValue());
			std::string overWriteValue = intToHexString(generateRandomIntValue());
#endif
			for (int j = 2; j <= 96; j += 2) {
				ssd->write(j, writeValue);
				ssd->write(j, overWriteValue);
				ssd->erase(j, 3);
				for (int k = 0; k < 3; k++) {
					if (!readCompare(j + k, "0x00000000")) {
						return false;
					}
				}
			}
		}
		PRINT_LOG("Done");
		return true;
	}
	catch (...) {
		PRINT_LOG("Fail! (exception)");
		return false;
	}
}
