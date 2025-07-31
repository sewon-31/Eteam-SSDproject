#include "ssd_interface.h"
#include <windows.h>
#include <sstream>
#include <iostream>
#include <fstream>

bool SSDDriver::runExe(const string& command) {
	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	std::istringstream iss(command);
	string exe;
	iss >> exe;

	string args;
	std::getline(iss, args); // args = " W 3 0x123456"

	string fullCommandLine = exe + args;

	char cmdLine[512];
	strcpy_s(cmdLine, fullCommandLine.c_str());

	BOOL result = CreateProcessA(
		exe.c_str(),      // execution file e.g. SSD.exe
		cmdLine,          // total cmd line
		NULL, NULL,
		FALSE,
		0,                // flag like CREATE_NO_WINDOW if needed
		NULL, NULL,
		&si, &pi
	);

	if (!result) {
		return false;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}

void SSDDriver::write(int lba, string value) {
	string command = "ssd.exe W " + std::to_string(lba) + " " + value;
	if (!runExe(command)) {
		throw SSDExecutionException("Execution failed: " + command);
	}
}

string SSDDriver::read(int lba) {
	string command = "ssd.exe R " + std::to_string(lba);
	if (!runExe(command)) {
		throw std::runtime_error("Failed to execute ssd.exe for read()");
	}

	std::ifstream file(SSD_READ_RESULT);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open result file: " + SSD_READ_RESULT);
	}

	string content;
	std::getline(file, content);
	file.close();

	return content;
}

void SSDDriver::erase(int lba, int size) {
	string command = "ssd.exe E " + std::to_string(lba) + " " + std::to_string(size);
	if (!runExe(command)) {
		throw SSDExecutionException("Execution failed: " + command);
	}
}

void SSDDriver::flush() {
	string command = "ssd.exe F";
	if (!runExe(command)) {
		throw SSDExecutionException("Execution failed: " + command);
	}
}
