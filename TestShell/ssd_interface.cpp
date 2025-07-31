#include "ssd_interface.h"

void SSDDriver::write(int lba, string value) {
	string command = "\"ssd W " + std::to_string(lba) + " " + value + " >nul 2>&1\"";
	if (runExe(command) == false) {
		throw SSDExecutionException("Execution failed: " + command);
	}
}

string SSDDriver::read(int lba) {
	string command = "\"ssd R " + std::to_string(lba) + " >nul 2>&1\"";
	if (runExe(command) == false) {
		throw std::runtime_error("There is no SSD.exe\n");
	}

	string content;
	std::ifstream file(SSD_READ_RESULT);
	std::getline(file, content);
	file.close();

	return content;
}

void SSDDriver::erase(int lba, int size) {
	string command = "\"ssd E " + std::to_string(lba) + " " + std::to_string(size) + " >nul 2>&1\"";
	if (runExe(command) == false) {
		throw SSDExecutionException("Execution failed: " + command);
	}
}

void SSDDriver::flush() {
	string command = "\"ssd F >nul 2>&1\"";
	if (runExe(command) == false) {
		throw SSDExecutionException("Execution failed: " + command);
	}
}
