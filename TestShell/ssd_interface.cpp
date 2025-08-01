#include "ssd_interface.h"
#include "file_util.h"
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
	if (!std::getline(iss, args)) {
		std::cerr << "[runExe] Failed to parse arguments from command: " << command << std::endl;
		return false;
	}

	string fullCommandLine = exe + args;

	char cmdLine[512];
	strcpy_s(cmdLine, fullCommandLine.c_str());

	BOOL result = CreateProcessA(
		exe.c_str(),      // application name e.g. SSD.exe
		cmdLine,          // full command line
		NULL, NULL,
		FALSE,
		0,
		NULL, NULL,
		&si, &pi
	);

	if (!result) {
		std::cerr << "CreateProcessA failed for command: " << fullCommandLine << std::endl;
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

	string content;
	if (!FileUtil::readLine(SSD_READ_RESULT, content)) {
		throw std::runtime_error("Failed to read line from result file: " + SSD_READ_RESULT);
	}

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
