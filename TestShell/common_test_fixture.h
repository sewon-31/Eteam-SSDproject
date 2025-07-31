#pragma once
#include <string>

class HandleConsoleOutputFixture {
public:
	std::string getLastLine(const std::string& str) {
		if (str.empty()) {
			return "";
		}

		size_t lastNewlinePos = str.find_last_of('\n');

		if (lastNewlinePos == str.length() - 1) {
			size_t secondLastNewlinePos = str.rfind('\n', lastNewlinePos - 1);
			if (secondLastNewlinePos == std::string::npos) {
				return str;
			}
			else {
				return str.substr(secondLastNewlinePos + 1, (lastNewlinePos - (secondLastNewlinePos + 1)) + 1);
			}
		}
		else if (lastNewlinePos != std::string::npos) {
			return str.substr(lastNewlinePos + 1);
		}
		else {
			return str;
		}
	}
	std::string excludeFirstLine(const std::string& str) {
		if (str.empty()) {
			return "";
		}

		size_t firstNewlinePos = str.find('\n');

		if (firstNewlinePos == std::string::npos) {
			return "";
		}

		return str.substr(firstNewlinePos + 1);
	}
};