#include "test_script1.h"

void TestScript1::Run(void) {
	string data = "0x12341234";

	for (int i = 0; i < (100 / 5); i++) {
		// Write
		for (int j = 0; j < 5; j++) {
			ssd.write(i*5+j, data);
		}

		// Read & Compare
		for (int j = 0; j < 5; j++) {
			string output = ssd.read(i * 5 + j);
			if (output != data)
				// Fail
				return;
		}
	}
}
