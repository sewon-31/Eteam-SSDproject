#include "command_buffer.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include <iostream>
#include <algorithm>
#include <sstream>

namespace fs = std::experimental::filesystem;

CommandBuffer::CommandBuffer(const string& dirPath)
	: bufferDirPath(dirPath)
{
	// create buffer directory (if needed)
	bool bufferDirExists = fs::exists(bufferDirPath) && fs::is_directory(bufferDirPath);

	if (!bufferDirExists) {
		if (fs::create_directory(bufferDirPath)) {
			for (int i = 1; i <= BUFFER_MAX; ++i) {
				string filePath = bufferDirPath + "/" + std::to_string(i) + "_empty";
				std::ofstream file(filePath);

				if (!file) {
#if _DEBUG
					std::cerr << "Failed to create " << filePath << std::endl;
#endif
				}
			}
		}
	}
}

void
CommandBuffer::updateFromDirectory()
{
	// read file names from directory
	vector<string> fileNames;

	for (const auto& file : fs::directory_iterator(bufferDirPath)) {
		if (fs::is_regular_file(file.path())) {
			fileNames.push_back(file.path().filename().string());
		}
	}

#if _DEBUG
	if (fileNames.size() != BUFFER_MAX) {
		std::cout << "Why not 5 files?\n";
	}
#endif

	// sort filenames
	std::sort(fileNames.begin(), fileNames.end());

	SSDCommandBuilder builder;

	auto startsWith = [](const string& str, const string& prefix) -> bool {
		return str.compare(0, prefix.size(), prefix) == 0;
		};

	int fileNum = 1;
	for (const auto& fileName : std::as_const(fileNames)) {
		string prefix = std::to_string(fileNum++) + "_";
		if (startsWith(fileName, prefix)) {
			string noPrefix = fileName.substr(prefix.length());

			if (noPrefix == EMPTY) {
				break;
			}

			vector<string> commandVector;
			std::stringstream ss(noPrefix);
			
			string token;
			while (std::getline(ss, token, '_')) {
				commandVector.push_back(token);
			}
			/*
			auto cmd = builder.createCommand(commandVector, ssd->getStorage());
			if (cmd == nullptr) {
				std::cout << "Commmand not created - " << noPrefix << std::endl;
			}
			buffer.push_back(cmd);
			*/
		}
		else {
			std::cout << "Weird file " << fileName << std::endl;
		}
	}
}

const std::vector<std::shared_ptr<ICommand>>&
CommandBuffer::getBuffer() const
{
	return buffer;
}

int
CommandBuffer::addCommand(std::shared_ptr<ICommand> command)
{
	buffer.push_back(command);
	optimizeBuffer();

	return buffer.size();
}

void
CommandBuffer::flushBuffer()
{
	//ssd.getStorage().updateFromFile();

	string result;
	for (auto cmd : buffer) {
		cmd->execute(result);
	}

	//ssd.getStorage().updateToFile();

	buffer.clear();
}

bool
CommandBuffer::optimizeBuffer()
{
	return true;
}